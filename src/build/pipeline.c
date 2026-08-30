/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/pipeline.c
 *
 * PURPOSE:
 *   Implement the reusable build-to-deployment composition state machine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/pipeline.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildDeliveryPipeline {
    char pipeline_id[UMI_BUILD_ID_CAPACITY];
    const UmiBuildTaskRegistry *task_registry;
    const UmiBuildDeploymentTargetRegistry *target_registry;
    UmiBuildIncrementalPlanner *incremental;
    UmiBuildTaskOrchestrator *tasks;
    UmiBuildArtifactManifestRegistry *artifacts;
    UmiBuildDeploymentController *deployment;
    UmiBuildPipelinePhase phase;
    uint64_t generation;
    uint64_t revision;
    int task_plan_available;
    int deployment_plan_available;
    int stop_requested;
};

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) (void)memcpy(destination, source, length);
    destination[length] = '\0';
}

static void refresh_task_phase(UmiBuildDeliveryPipeline *pipeline)
{
    UmiBuildTaskOperationSnapshot snapshot;
    if (umi_build_task_orchestrator_snapshot(pipeline->tasks, &snapshot) !=
        UMI_STATUS_OK) {
        return;
    }
    switch (snapshot.state) {
    case UMI_BUILD_TASK_OPERATION_SUCCEEDED:
        pipeline->phase = UMI_BUILD_PIPELINE_ARTIFACTS;
        break;
    case UMI_BUILD_TASK_OPERATION_FAILED:
        pipeline->phase = UMI_BUILD_PIPELINE_FAILED;
        break;
    case UMI_BUILD_TASK_OPERATION_CANCELLED:
        pipeline->phase = UMI_BUILD_PIPELINE_CANCELLED;
        break;
    default:
        break;
    }
}

static void refresh_deployment_phase(UmiBuildDeliveryPipeline *pipeline)
{
    UmiBuildDeploymentControllerSnapshot snapshot;
    if (umi_build_deployment_controller_snapshot(
            pipeline->deployment, &snapshot) != UMI_STATUS_OK) {
        return;
    }
    switch (snapshot.state) {
    case UMI_BUILD_DEPLOYMENT_SUCCEEDED:
        pipeline->phase = UMI_BUILD_PIPELINE_SUCCEEDED;
        break;
    case UMI_BUILD_DEPLOYMENT_FAILED:
        pipeline->phase = UMI_BUILD_PIPELINE_FAILED;
        break;
    case UMI_BUILD_DEPLOYMENT_CANCELLED:
        pipeline->phase = UMI_BUILD_PIPELINE_CANCELLED;
        break;
    default:
        break;
    }
}

UmiStatus umi_build_delivery_pipeline_create(
    const char *pipeline_id,
    const UmiBuildTaskRegistry *tasks,
    const UmiBuildDeploymentTargetRegistry *targets,
    size_t maximum_parallel,
    UmiBuildDeliveryPipeline **out_pipeline)
{
    UmiBuildDeliveryPipeline *pipeline;
    UmiStatus status;
    if (pipeline_id == NULL || pipeline_id[0] == '\0' || tasks == NULL ||
        targets == NULL || out_pipeline == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_pipeline = NULL;
    pipeline = (UmiBuildDeliveryPipeline *)calloc(1U, sizeof(*pipeline));
    if (pipeline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    copy_text(pipeline->pipeline_id, sizeof(pipeline->pipeline_id), pipeline_id);
    pipeline->task_registry = tasks;
    pipeline->target_registry = targets;
    pipeline->phase = UMI_BUILD_PIPELINE_IDLE;
    pipeline->revision = 1U;
    status = umi_build_incremental_planner_create(&pipeline->incremental);
    if (status == UMI_STATUS_OK) {
        status = umi_build_task_orchestrator_create(
            tasks, maximum_parallel, &pipeline->tasks);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_build_artifact_manifest_registry_create(
            &pipeline->artifacts);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_build_deployment_controller_create(&pipeline->deployment);
    }
    if (status != UMI_STATUS_OK) {
        umi_build_delivery_pipeline_destroy(pipeline);
        return status;
    }
    *out_pipeline = pipeline;
    return UMI_STATUS_OK;
}

void umi_build_delivery_pipeline_destroy(UmiBuildDeliveryPipeline *pipeline)
{
    if (pipeline == NULL) return;
    umi_build_deployment_controller_destroy(pipeline->deployment);
    umi_build_artifact_manifest_registry_destroy(pipeline->artifacts);
    umi_build_task_orchestrator_destroy(pipeline->tasks);
    umi_build_incremental_planner_destroy(pipeline->incremental);
    free(pipeline);
}

UmiStatus umi_build_delivery_pipeline_plan_tasks(
    UmiBuildDeliveryPipeline *pipeline,
    const char *operation_id,
    const char *task_id)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pipeline->phase == UMI_BUILD_PIPELINE_TASKS ||
        pipeline->phase == UMI_BUILD_PIPELINE_DEPLOYMENT) {
        return UMI_STATUS_BUSY;
    }
    status = umi_build_task_orchestrator_plan(
        pipeline->tasks, operation_id, task_id);
    if (status == UMI_STATUS_OK) {
        pipeline->task_plan_available = 1;
        pipeline->deployment_plan_available = 0;
        pipeline->stop_requested = 0;
        pipeline->phase = UMI_BUILD_PIPELINE_PLANNED;
        pipeline->generation += 1U;
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_begin_tasks(
    UmiBuildDeliveryPipeline *pipeline)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!pipeline->task_plan_available ||
        pipeline->phase != UMI_BUILD_PIPELINE_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_build_task_orchestrator_begin(pipeline->tasks);
    if (status == UMI_STATUS_OK) {
        pipeline->phase = UMI_BUILD_PIPELINE_TASKS;
        pipeline->revision += 1U;
        refresh_task_phase(pipeline);
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_next_task(
    UmiBuildDeliveryPipeline *pipeline,
    UmiBuildTaskSnapshot *out_task)
{
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pipeline->phase != UMI_BUILD_PIPELINE_TASKS) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_build_task_orchestrator_next_ready(pipeline->tasks, out_task);
}

UmiStatus umi_build_delivery_pipeline_start_task(
    UmiBuildDeliveryPipeline *pipeline,
    const char *task_id)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pipeline->phase != UMI_BUILD_PIPELINE_TASKS) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_build_task_orchestrator_start(pipeline->tasks, task_id);
    if (status == UMI_STATUS_OK) pipeline->revision += 1U;
    return status;
}

UmiStatus umi_build_delivery_pipeline_complete_task(
    UmiBuildDeliveryPipeline *pipeline,
    const char *task_id,
    int exit_code)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_task_orchestrator_complete(
        pipeline->tasks, task_id, exit_code);
    if (status == UMI_STATUS_OK) {
        refresh_task_phase(pipeline);
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_fail_task(
    UmiBuildDeliveryPipeline *pipeline,
    const char *task_id,
    UmiStatus status,
    int exit_code)
{
    UmiStatus result;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_build_task_orchestrator_fail(
        pipeline->tasks, task_id, status, exit_code);
    if (result == UMI_STATUS_OK) {
        refresh_task_phase(pipeline);
        pipeline->revision += 1U;
    }
    return result;
}

UmiStatus umi_build_delivery_pipeline_register_artifact(
    UmiBuildDeliveryPipeline *pipeline,
    const UmiBuildArtifactManifest *artifact)
{
    UmiStatus status;
    if (pipeline == NULL || artifact == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (pipeline->phase != UMI_BUILD_PIPELINE_ARTIFACTS &&
        pipeline->phase != UMI_BUILD_PIPELINE_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_build_artifact_manifest_registry_upsert(
        pipeline->artifacts, artifact);
    if (status == UMI_STATUS_OK) {
        pipeline->phase = UMI_BUILD_PIPELINE_ARTIFACTS;
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_plan_deployment(
    UmiBuildDeliveryPipeline *pipeline,
    const char *deployment_id,
    const char *artifact_id,
    const char *target_id,
    int dry_run)
{
    UmiBuildArtifactManifest artifact;
    UmiBuildDeploymentTargetSnapshot target;
    UmiBuildDeploymentPlan plan;
    UmiStatus status;
    if (pipeline == NULL || deployment_id == NULL || artifact_id == NULL ||
        target_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (pipeline->phase != UMI_BUILD_PIPELINE_ARTIFACTS &&
        pipeline->phase != UMI_BUILD_PIPELINE_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_build_artifact_manifest_registry_find(
        pipeline->artifacts, artifact_id, &artifact);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_deployment_target_registry_find(
        pipeline->target_registry, target_id, &target);
    if (status != UMI_STATUS_OK) return status;
    umi_build_deployment_plan_init(&plan, deployment_id);
    status = umi_build_deployment_plan_compose_default(
        &plan, &artifact, &target, dry_run);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_deployment_controller_load(pipeline->deployment, &plan);
    if (status == UMI_STATUS_OK) {
        pipeline->deployment_plan_available = 1;
        pipeline->phase = UMI_BUILD_PIPELINE_PLANNED;
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_approve_deployment(
    UmiBuildDeliveryPipeline *pipeline)
{
    UmiBuildDeploymentPlan plan;
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!pipeline->deployment_plan_available) return UMI_STATUS_INVALID_STATE;
    status = umi_build_deployment_controller_plan(pipeline->deployment, &plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_deployment_plan_approve(&plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_deployment_controller_load(pipeline->deployment, &plan);
    if (status == UMI_STATUS_OK) pipeline->revision += 1U;
    return status;
}

UmiStatus umi_build_delivery_pipeline_begin_deployment(
    UmiBuildDeliveryPipeline *pipeline)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!pipeline->deployment_plan_available ||
        pipeline->phase != UMI_BUILD_PIPELINE_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_build_deployment_controller_begin(pipeline->deployment);
    if (status == UMI_STATUS_OK) {
        pipeline->phase = UMI_BUILD_PIPELINE_DEPLOYMENT;
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_next_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    UmiBuildDeploymentStepSnapshot *out_step)
{
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (pipeline->phase != UMI_BUILD_PIPELINE_DEPLOYMENT) {
        return UMI_STATUS_INVALID_STATE;
    }
    return umi_build_deployment_controller_next_step(
        pipeline->deployment, out_step);
}

UmiStatus umi_build_delivery_pipeline_start_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    const char *step_id)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_deployment_controller_start_step(
        pipeline->deployment, step_id);
    if (status == UMI_STATUS_OK) pipeline->revision += 1U;
    return status;
}

UmiStatus umi_build_delivery_pipeline_complete_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    const char *step_id,
    int exit_code)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_build_deployment_controller_complete_step(
        pipeline->deployment, step_id, exit_code);
    if (status == UMI_STATUS_OK) {
        refresh_deployment_phase(pipeline);
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_fail_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    const char *step_id,
    UmiStatus status,
    int exit_code,
    const char *message)
{
    UmiStatus result;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = umi_build_deployment_controller_fail_step(
        pipeline->deployment, step_id, status, exit_code, message);
    if (result == UMI_STATUS_OK) {
        refresh_deployment_phase(pipeline);
        pipeline->revision += 1U;
    }
    return result;
}

UmiStatus umi_build_delivery_pipeline_request_stop(
    UmiBuildDeliveryPipeline *pipeline)
{
    UmiStatus status;
    if (pipeline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    pipeline->stop_requested = 1;
    if (pipeline->phase == UMI_BUILD_PIPELINE_TASKS) {
        status = umi_build_task_orchestrator_request_stop(pipeline->tasks);
    } else if (pipeline->phase == UMI_BUILD_PIPELINE_DEPLOYMENT) {
        status = umi_build_deployment_controller_request_stop(
            pipeline->deployment);
    } else {
        return UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) {
        pipeline->phase = UMI_BUILD_PIPELINE_CANCELLED;
        pipeline->revision += 1U;
    }
    return status;
}

UmiStatus umi_build_delivery_pipeline_snapshot(
    const UmiBuildDeliveryPipeline *pipeline,
    UmiBuildDeliveryPipelineSnapshot *out_snapshot)
{
    UmiStatus status;
    if (pipeline == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_BUILD_PIPELINE_API_VERSION;
    copy_text(out_snapshot->pipeline_id, sizeof(out_snapshot->pipeline_id),
              pipeline->pipeline_id);
    out_snapshot->phase = pipeline->phase;
    out_snapshot->generation = pipeline->generation;
    out_snapshot->revision = pipeline->revision;
    out_snapshot->task_plan_available = pipeline->task_plan_available;
    out_snapshot->deployment_plan_available =
        pipeline->deployment_plan_available;
    out_snapshot->stop_requested = pipeline->stop_requested;
    out_snapshot->available = 1;
    status = umi_build_incremental_planner_snapshot(
        pipeline->incremental, &out_snapshot->incremental);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_task_orchestrator_snapshot(
        pipeline->tasks, &out_snapshot->tasks);
    if (status != UMI_STATUS_OK) return status;
    status = umi_build_artifact_manifest_registry_snapshot(
        pipeline->artifacts, &out_snapshot->artifacts);
    if (status != UMI_STATUS_OK) return status;
    return umi_build_deployment_controller_snapshot(
        pipeline->deployment, &out_snapshot->deployment);
}

UmiBuildIncrementalPlanner *umi_build_delivery_pipeline_incremental(
    UmiBuildDeliveryPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->incremental : NULL;
}

UmiBuildTaskOrchestrator *umi_build_delivery_pipeline_tasks(
    UmiBuildDeliveryPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->tasks : NULL;
}

UmiBuildArtifactManifestRegistry *umi_build_delivery_pipeline_artifacts(
    UmiBuildDeliveryPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->artifacts : NULL;
}

UmiBuildDeploymentController *umi_build_delivery_pipeline_deployment(
    UmiBuildDeliveryPipeline *pipeline)
{
    return pipeline != NULL ? pipeline->deployment : NULL;
}

const char *umi_build_pipeline_phase_text(UmiBuildPipelinePhase phase)
{
    switch (phase) {
    case UMI_BUILD_PIPELINE_IDLE: return "Idle";
    case UMI_BUILD_PIPELINE_PLANNED: return "Planned";
    case UMI_BUILD_PIPELINE_TASKS: return "Tasks";
    case UMI_BUILD_PIPELINE_ARTIFACTS: return "Artifacts";
    case UMI_BUILD_PIPELINE_DEPLOYMENT: return "Deployment";
    case UMI_BUILD_PIPELINE_SUCCEEDED: return "Succeeded";
    case UMI_BUILD_PIPELINE_FAILED: return "Failed";
    case UMI_BUILD_PIPELINE_CANCELLED: return "Cancelled";
    default: return "Unknown";
    }
}
