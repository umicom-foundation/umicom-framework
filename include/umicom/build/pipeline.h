/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/pipeline.h
 *
 * PURPOSE:
 *   Coordinate incremental selection, task execution, artifact evidence and
 *   deployment supervision as one reusable delivery pipeline.
 *
 * ARCHITECTURE:
 *   Every specialised model remains authoritative in its own module. The
 *   pipeline owns composition and phase transitions only. It never parses a
 *   shell command, writes a package or opens a network connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BUILD_PIPELINE_H
#define UMICOM_BUILD_PIPELINE_H

#include "umicom/build/deployment_controller.h"
#include "umicom/build/incremental.h"
#include "umicom/build/task_orchestrator.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BUILD_PIPELINE_API_VERSION 1U

typedef enum UmiBuildPipelinePhase {
    UMI_BUILD_PIPELINE_IDLE = 0,
    UMI_BUILD_PIPELINE_PLANNED = 1,
    UMI_BUILD_PIPELINE_TASKS = 2,
    UMI_BUILD_PIPELINE_ARTIFACTS = 3,
    UMI_BUILD_PIPELINE_DEPLOYMENT = 4,
    UMI_BUILD_PIPELINE_SUCCEEDED = 5,
    UMI_BUILD_PIPELINE_FAILED = 6,
    UMI_BUILD_PIPELINE_CANCELLED = 7
} UmiBuildPipelinePhase;

typedef struct UmiBuildDeliveryPipelineSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    char pipeline_id[UMI_BUILD_ID_CAPACITY];
    UmiBuildPipelinePhase phase;
    UmiBuildIncrementalSnapshot incremental;
    UmiBuildTaskOperationSnapshot tasks;
    UmiBuildArtifactManifestRegistrySnapshot artifacts;
    UmiBuildDeploymentControllerSnapshot deployment;
    uint64_t generation;
    uint64_t revision;
    int task_plan_available;
    int deployment_plan_available;
    int stop_requested;
    int available;
} UmiBuildDeliveryPipelineSnapshot;

typedef struct UmiBuildDeliveryPipeline UmiBuildDeliveryPipeline;

/* Task and target registries are borrowed and must outlive this owner.
 * Serialize pipeline calls and child observation on the workspace owner thread.
 * Identities must fit in full; construction never truncates a different ID. */
UmiStatus umi_build_delivery_pipeline_create(
    const char *pipeline_id,
    const UmiBuildTaskRegistry *tasks,
    const UmiBuildDeploymentTargetRegistry *targets,
    size_t maximum_parallel,
    UmiBuildDeliveryPipeline **out_pipeline);
void umi_build_delivery_pipeline_destroy(UmiBuildDeliveryPipeline *pipeline);
/* Mutations reserve the parent revision (and planning generation) before
 * changing a child. Refusal preserves the accepted parent and child state.
 * Planning a deployment selects the deployment branch of PLANNED; begin_tasks
 * cannot restart its older task plan. Replacing an artifact invalidates a
 * previously composed deployment plan, which must be reviewed again. */
UmiStatus umi_build_delivery_pipeline_plan_tasks(
    UmiBuildDeliveryPipeline *pipeline,
    const char *operation_id,
    const char *task_id);
UmiStatus umi_build_delivery_pipeline_begin_tasks(
    UmiBuildDeliveryPipeline *pipeline);
UmiStatus umi_build_delivery_pipeline_next_task(
    UmiBuildDeliveryPipeline *pipeline,
    UmiBuildTaskSnapshot *out_task);
UmiStatus umi_build_delivery_pipeline_start_task(
    UmiBuildDeliveryPipeline *pipeline,
    const char *task_id);
UmiStatus umi_build_delivery_pipeline_complete_task(
    UmiBuildDeliveryPipeline *pipeline,
    const char *task_id,
    int exit_code);
UmiStatus umi_build_delivery_pipeline_fail_task(
    UmiBuildDeliveryPipeline *pipeline,
    const char *task_id,
    UmiStatus status,
    int exit_code);
UmiStatus umi_build_delivery_pipeline_register_artifact(
    UmiBuildDeliveryPipeline *pipeline,
    const UmiBuildArtifactManifest *artifact);
UmiStatus umi_build_delivery_pipeline_plan_deployment(
    UmiBuildDeliveryPipeline *pipeline,
    const char *deployment_id,
    const char *artifact_id,
    const char *target_id,
    int dry_run);
UmiStatus umi_build_delivery_pipeline_approve_deployment(
    UmiBuildDeliveryPipeline *pipeline);
UmiStatus umi_build_delivery_pipeline_begin_deployment(
    UmiBuildDeliveryPipeline *pipeline);
UmiStatus umi_build_delivery_pipeline_next_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    UmiBuildDeploymentStepSnapshot *out_step);
UmiStatus umi_build_delivery_pipeline_start_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    const char *step_id);
UmiStatus umi_build_delivery_pipeline_complete_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    const char *step_id,
    int exit_code);
UmiStatus umi_build_delivery_pipeline_fail_deployment_step(
    UmiBuildDeliveryPipeline *pipeline,
    const char *step_id,
    UmiStatus status,
    int exit_code,
    const char *message);
/* Stop requests prevent new scheduling; they do not kill an OS process.
 * While a child task or deployment step is active the phase remains TASKS or
 * DEPLOYMENT and stop_requested is true. Supply its actual completion/failure
 * callback before the child can publish its terminal outcome. A refused stop
 * changes neither owner. Do not destroy this owner while callbacks are pending. */
UmiStatus umi_build_delivery_pipeline_request_stop(
    UmiBuildDeliveryPipeline *pipeline);
/* Publish a complete composed observation or leave output unchanged. Output
 * storage must be separate from the owner and all its borrowed children. */
UmiStatus umi_build_delivery_pipeline_snapshot(
    const UmiBuildDeliveryPipeline *pipeline,
    UmiBuildDeliveryPipelineSnapshot *out_snapshot);
/* Borrowed compatibility accessors. Observe these children; route lifecycle
 * changes through the pipeline so parent phases and revisions stay consistent.
 * Metadata and successful model transitions are not evidence that commands,
 * artifact checksums, installation or external deployment have executed. */
UmiBuildIncrementalPlanner *umi_build_delivery_pipeline_incremental(
    UmiBuildDeliveryPipeline *pipeline);
UmiBuildTaskOrchestrator *umi_build_delivery_pipeline_tasks(
    UmiBuildDeliveryPipeline *pipeline);
UmiBuildArtifactManifestRegistry *umi_build_delivery_pipeline_artifacts(
    UmiBuildDeliveryPipeline *pipeline);
UmiBuildDeploymentController *umi_build_delivery_pipeline_deployment(
    UmiBuildDeliveryPipeline *pipeline);
const char *umi_build_pipeline_phase_text(UmiBuildPipelinePhase phase);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_BUILD_PIPELINE_H */
