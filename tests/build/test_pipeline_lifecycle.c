/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build/test_pipeline_lifecycle.c
 *
 * PURPOSE:
 *   Verify delivery cancellation, identity admission and parent/child mutation boundaries without executing tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Private inclusion exposes exhausted parent identities without adding mutation
 * hooks to the public SDK. Only selected nested failures are substituted; the
 * real graph, registry and deployment owners remain linked Framework services. */
static int refuse_stop;
static int refuse_snapshot;
static UmiStatus TestTaskStop(UmiBuildTaskOrchestrator *owner)
{
    return refuse_stop ? UMI_STATUS_CAPACITY_EXCEEDED : umi_build_task_orchestrator_request_stop(owner);
}
static UmiStatus TestDeploymentStop(UmiBuildDeploymentController *owner)
{
    return refuse_stop ? UMI_STATUS_CAPACITY_EXCEEDED : umi_build_deployment_controller_request_stop(owner);
}
static UmiStatus TestIncrementalSnapshot(const UmiBuildIncrementalPlanner *owner, UmiBuildIncrementalSnapshot *out)
{
    return refuse_snapshot == 1 ? UMI_STATUS_INVALID_STATE : umi_build_incremental_planner_snapshot(owner, out);
}
static UmiStatus TestTaskSnapshot(const UmiBuildTaskOrchestrator *owner, UmiBuildTaskOperationSnapshot *out)
{
    return refuse_snapshot == 2 ? UMI_STATUS_INVALID_STATE : umi_build_task_orchestrator_snapshot(owner, out);
}
static UmiStatus TestArtifactSnapshot(const UmiBuildArtifactManifestRegistry *owner, UmiBuildArtifactManifestRegistrySnapshot *out)
{
    return refuse_snapshot == 3 ? UMI_STATUS_INVALID_STATE : umi_build_artifact_manifest_registry_snapshot(owner, out);
}
static UmiStatus TestDeploymentSnapshot(const UmiBuildDeploymentController *owner, UmiBuildDeploymentControllerSnapshot *out)
{
    return refuse_snapshot == 4 ? UMI_STATUS_INVALID_STATE : umi_build_deployment_controller_snapshot(owner, out);
}
#define umi_build_task_orchestrator_request_stop TestTaskStop
#define umi_build_deployment_controller_request_stop TestDeploymentStop
#define umi_build_incremental_planner_snapshot TestIncrementalSnapshot
#define umi_build_task_orchestrator_snapshot TestTaskSnapshot
#define umi_build_artifact_manifest_registry_snapshot TestArtifactSnapshot
#define umi_build_deployment_controller_snapshot TestDeploymentSnapshot
#include "../../src/build/pipeline.c"
#undef umi_build_task_orchestrator_request_stop
#undef umi_build_deployment_controller_request_stop
#undef umi_build_incremental_planner_snapshot
#undef umi_build_task_orchestrator_snapshot
#undef umi_build_artifact_manifest_registry_snapshot
#undef umi_build_deployment_controller_snapshot

#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
static int Is(const char *a, const char *b) { return strcmp(a, b) == 0; }

/* Descriptions only: no shell, filesystem, network or installation is invoked. */
static UmiStatus AddTask(UmiBuildTaskRegistry *tasks)
{
    UmiBuildTaskSnapshot task;
    umi_build_task_init(&task, "build", "Build fixture", UMI_BUILD_TASK_BUILD);
    UmiStatus status = umi_build_task_set_command(&task, "fixture-compiler", ".");
    return status == UMI_STATUS_OK ? umi_build_task_registry_upsert(tasks, &task) : status;
}
static UmiStatus AddTarget(UmiBuildDeploymentTargetRegistry *targets)
{
    UmiBuildDeploymentTargetSnapshot target;
    umi_build_deployment_target_init(&target, "local", "Local fixture", UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY);
    strcpy(target.destination, "fixture/install");
    return umi_build_deployment_target_registry_upsert(targets, &target);
}
static void Artifact(UmiBuildArtifactManifest *artifact)
{
    umi_build_artifact_manifest_init(artifact, "package", "fixture/package.zip", "archive");
    artifact->flags |= UMI_BUILD_ARTIFACT_PRIMARY | UMI_BUILD_ARTIFACT_DEPLOYABLE | UMI_BUILD_ARTIFACT_VERIFIED;
    strcpy(artifact->checksum_algorithm, "SHA-256");
    memset(artifact->checksum, 'a', 64); artifact->checksum[64] = '\0';
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    UmiBuildTaskRegistry *tasks = NULL;
    UmiBuildDeploymentTargetRegistry *targets = NULL;
    UmiBuildDeliveryPipeline *owner = NULL;
    REQUIRE(umi_build_task_registry_create(&tasks) == UMI_STATUS_OK);
    REQUIRE(AddTask(tasks) == UMI_STATUS_OK);
    REQUIRE(umi_build_deployment_target_registry_create(&targets) == UMI_STATUS_OK);
    REQUIRE(AddTarget(targets) == UMI_STATUS_OK);
    char long_id[UMI_BUILD_ID_CAPACITY + 1];
    memset(long_id, 'x', sizeof long_id - 1); long_id[sizeof long_id - 1] = '\0';
    if (Is(mode, "id-long") || Is(mode, "id-boundary") || Is(mode, "id-empty")) {
        if (Is(mode, "id-boundary")) long_id[UMI_BUILD_ID_CAPACITY - 1] = '\0';
        UmiStatus expected = Is(mode, "id-long") ? UMI_STATUS_CAPACITY_EXCEEDED : Is(mode, "id-empty") ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
        REQUIRE(umi_build_delivery_pipeline_create(Is(mode, "id-empty") ? "" : long_id, tasks, targets, 1, &owner) == expected);
        if (owner != NULL) REQUIRE(strcmp(owner->pipeline_id, long_id) == 0);
        goto done;
    }
    REQUIRE(umi_build_delivery_pipeline_create("delivery", tasks, targets, 1, &owner) == UMI_STATUS_OK);
    UmiBuildDeliveryPipelineSnapshot before, after;
    memset(&before, 0, sizeof before); memset(&after, 0, sizeof after);
    UmiBuildArtifactManifest artifact; Artifact(&artifact);
    UmiBuildDeploymentStepSnapshot step = {0};
    if (strncmp(mode, "snapshot-", 9) == 0) {
        memset(&after, 0xa5, sizeof after); memcpy(&before, &after, sizeof before);
        if (Is(mode, "snapshot-incremental")) refuse_snapshot = 1;
        else if (Is(mode, "snapshot-tasks")) refuse_snapshot = 2;
        else if (Is(mode, "snapshot-artifacts")) refuse_snapshot = 3;
        else if (Is(mode, "snapshot-deployment")) refuse_snapshot = 4;
        else return 2;
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &after, sizeof after) == 0);
        goto done;
    }
    if (Is(mode, "stop-idle")) {
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        REQUIRE(umi_build_delivery_pipeline_request_stop(owner) == UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
        goto done;
    }
    int stage = -1;
    if (Is(mode, "stop-planned")) stage = 1;
    if (Is(mode, "stop-task-waiting")) stage = 2;
    if (Is(mode, "stop-task-running")) stage = 3;
    if (Is(mode, "stop-task-failure")) stage = 3;
    if (Is(mode, "stop-task-refusal")) stage = 3;
    if (Is(mode, "stop-deployment-waiting")) stage = 7;
    if (Is(mode, "stop-deployment-running")) stage = 8;
    if (Is(mode, "stop-deployment-failure")) stage = 8;
    if (Is(mode, "stop-deployment-refusal")) stage = 8;
    if (Is(mode, "artifact-replacement")) stage = 6;
    if (Is(mode, "tasks-after-deployment-plan")) stage = 6;
    if (Is(mode, "deployment-id-long")) stage = 5;
    if (Is(mode, "artifact-id-long")) stage = 5;
    if (Is(mode, "target-id-long")) stage = 5;
    if (Is(mode, "deployment-id-empty")) stage = 5;
    if (Is(mode, "dry-run-invalid")) stage = 5;
    if (Is(mode, "plan-generation")) stage = 0;
    if (Is(mode, "generation-boundary")) stage = 0;
    if (Is(mode, "revision-boundary")) stage = 1;
    if (Is(mode, "revision-plan")) stage = 0;
    if (Is(mode, "revision-begin-tasks")) stage = 1;
    if (Is(mode, "revision-start-task")) stage = 2;
    if (Is(mode, "revision-complete-task")) stage = 3;
    if (Is(mode, "revision-fail-task")) stage = 3;
    if (Is(mode, "revision-stop-task")) stage = 3;
    if (Is(mode, "revision-artifact")) stage = 4;
    if (Is(mode, "revision-plan-deployment")) stage = 5;
    if (Is(mode, "revision-approve-deployment")) stage = 6;
    if (Is(mode, "revision-begin-deployment")) stage = 6;
    if (Is(mode, "revision-start-step")) stage = 7;
    if (Is(mode, "revision-complete-step")) stage = 8;
    if (Is(mode, "revision-fail-step")) stage = 8;
    if (Is(mode, "revision-stop-deployment")) stage = 8;
    if (stage < 0) return 2;
    if (stage >= 1) REQUIRE(umi_build_delivery_pipeline_plan_tasks(owner, "operation", "build") == UMI_STATUS_OK);
    if (stage >= 2) REQUIRE(umi_build_delivery_pipeline_begin_tasks(owner) == UMI_STATUS_OK);
    if (stage >= 3) REQUIRE(umi_build_delivery_pipeline_start_task(owner, "build") == UMI_STATUS_OK);
    if (stage >= 4) REQUIRE(umi_build_delivery_pipeline_complete_task(owner, "build", 0) == UMI_STATUS_OK);
    if (stage >= 5) REQUIRE(umi_build_delivery_pipeline_register_artifact(owner, &artifact) == UMI_STATUS_OK);
    if (stage >= 6) REQUIRE(umi_build_delivery_pipeline_plan_deployment(owner, "deployment", "package", "local", 1) == UMI_STATUS_OK);
    if (stage >= 7) {
        REQUIRE(umi_build_delivery_pipeline_begin_deployment(owner) == UMI_STATUS_OK);
        REQUIRE(umi_build_delivery_pipeline_next_deployment_step(owner, &step) == UMI_STATUS_OK);
    }
    if (stage >= 8) REQUIRE(umi_build_delivery_pipeline_start_deployment_step(owner, step.step_id) == UMI_STATUS_OK);
    if (Is(mode, "generation-boundary")) {
        owner->generation = UINT64_MAX - 1;
        REQUIRE(umi_build_delivery_pipeline_plan_tasks(owner, "last", "build") == UMI_STATUS_OK);
        REQUIRE(owner->generation == UINT64_MAX);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        REQUIRE(umi_build_delivery_pipeline_plan_tasks(owner, "overflow", "build") == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
        goto done;
    }
    if (Is(mode, "revision-boundary")) {
        owner->revision = UINT64_MAX - 1;
        REQUIRE(umi_build_delivery_pipeline_begin_tasks(owner) == UMI_STATUS_OK);
        REQUIRE(owner->revision == UINT64_MAX);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        REQUIRE(umi_build_delivery_pipeline_start_task(owner, "build") == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
        goto done;
    }
    if (strncmp(mode, "revision-", 9) == 0 || Is(mode, "plan-generation")) {
        if (Is(mode, "plan-generation")) owner->generation = UINT64_MAX;
        else owner->revision = UINT64_MAX;
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        UmiStatus result = UMI_STATUS_OK;
        if (Is(mode, "revision-plan")) result = umi_build_delivery_pipeline_plan_tasks(owner, "replacement", "build");
        if (Is(mode, "revision-begin-tasks")) result = umi_build_delivery_pipeline_begin_tasks(owner);
        if (Is(mode, "revision-start-task")) result = umi_build_delivery_pipeline_start_task(owner, "build");
        if (Is(mode, "revision-complete-task")) result = umi_build_delivery_pipeline_complete_task(owner, "build", 0);
        if (Is(mode, "revision-fail-task")) result = umi_build_delivery_pipeline_fail_task(owner, "build", UMI_STATUS_INTERNAL_ERROR, 7);
        if (Is(mode, "revision-stop-task")) result = umi_build_delivery_pipeline_request_stop(owner);
        if (Is(mode, "revision-artifact")) result = umi_build_delivery_pipeline_register_artifact(owner, &artifact);
        if (Is(mode, "revision-plan-deployment")) result = umi_build_delivery_pipeline_plan_deployment(owner, "deployment", "package", "local", 1);
        if (Is(mode, "revision-approve-deployment")) result = umi_build_delivery_pipeline_approve_deployment(owner);
        if (Is(mode, "revision-begin-deployment")) result = umi_build_delivery_pipeline_begin_deployment(owner);
        if (Is(mode, "revision-start-step")) result = umi_build_delivery_pipeline_start_deployment_step(owner, step.step_id);
        if (Is(mode, "revision-complete-step")) result = umi_build_delivery_pipeline_complete_deployment_step(owner, step.step_id, 0);
        if (Is(mode, "revision-fail-step")) result = umi_build_delivery_pipeline_fail_deployment_step(owner, step.step_id, UMI_STATUS_INTERNAL_ERROR, 7, "fixture failure");
        if (Is(mode, "revision-stop-deployment")) result = umi_build_delivery_pipeline_request_stop(owner);
        if (Is(mode, "plan-generation")) result = umi_build_delivery_pipeline_plan_tasks(owner, "replacement", "build");
        REQUIRE(result == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
    } else if (Is(mode, "stop-planned") || Is(mode, "stop-task-refusal") || Is(mode, "stop-deployment-refusal")) {
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        refuse_stop = !Is(mode, "stop-planned");
        REQUIRE(umi_build_delivery_pipeline_request_stop(owner) == (refuse_stop ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE));
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
    } else if (strncmp(mode, "stop-", 5) == 0) {
        int task_phase = strncmp(mode, "stop-task-", 10) == 0;
        int waiting = strstr(mode, "waiting") != NULL;
        int fail = strstr(mode, "failure") != NULL;
        REQUIRE(umi_build_delivery_pipeline_request_stop(owner) == UMI_STATUS_OK);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(after.stop_requested == 1);
        if (waiting) REQUIRE(after.phase == UMI_BUILD_PIPELINE_CANCELLED);
        else {
            REQUIRE(after.phase == (task_phase ? UMI_BUILD_PIPELINE_TASKS : UMI_BUILD_PIPELINE_DEPLOYMENT));
            REQUIRE(umi_build_delivery_pipeline_plan_tasks(owner, "premature", "build") == UMI_STATUS_BUSY);
            if (task_phase) {
                REQUIRE(after.tasks.active_count == 1);
                REQUIRE(umi_build_delivery_pipeline_start_task(owner, "build") == UMI_STATUS_INVALID_STATE);
                REQUIRE((fail ? umi_build_delivery_pipeline_fail_task(owner, "build", UMI_STATUS_INTERNAL_ERROR, 7) :
                    umi_build_delivery_pipeline_complete_task(owner, "build", 0)) == UMI_STATUS_OK);
            } else {
                REQUIRE(after.deployment.current_step_id[0] != '\0');
                REQUIRE(umi_build_delivery_pipeline_start_deployment_step(owner, step.step_id) == UMI_STATUS_INVALID_STATE);
                REQUIRE((fail ? umi_build_delivery_pipeline_fail_deployment_step(owner, step.step_id, UMI_STATUS_INTERNAL_ERROR, 7, "fixture failure") :
                    umi_build_delivery_pipeline_complete_deployment_step(owner, step.step_id, 0)) == UMI_STATUS_OK);
            }
            REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
            REQUIRE(after.phase == (!task_phase && fail ? UMI_BUILD_PIPELINE_FAILED : UMI_BUILD_PIPELINE_CANCELLED));
            if (task_phase) REQUIRE(after.tasks.active_count == 0);
        }
    } else if (Is(mode, "artifact-replacement")) {
        REQUIRE(owner->deployment_plan_available == 1);
        REQUIRE(umi_build_delivery_pipeline_register_artifact(owner, &artifact) == UMI_STATUS_OK);
        REQUIRE(owner->deployment_plan_available == 0 && owner->phase == UMI_BUILD_PIPELINE_ARTIFACTS);
        REQUIRE(umi_build_delivery_pipeline_begin_deployment(owner) == UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_build_delivery_pipeline_plan_deployment(owner, "replacement", "package", "local", 1) == UMI_STATUS_OK);
    } else if (Is(mode, "tasks-after-deployment-plan")) {
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        REQUIRE(umi_build_delivery_pipeline_begin_tasks(owner) == UMI_STATUS_INVALID_STATE);
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
    } else {
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &before) == UMI_STATUS_OK);
        UmiStatus result = umi_build_delivery_pipeline_plan_deployment(owner,
            Is(mode, "deployment-id-long") ? long_id : Is(mode, "deployment-id-empty") ? "" : "replacement",
            Is(mode, "artifact-id-long") ? long_id : "package",
            Is(mode, "target-id-long") ? long_id : "local", Is(mode, "dry-run-invalid") ? 2 : 1);
        REQUIRE(result == ((Is(mode, "deployment-id-empty") || Is(mode, "dry-run-invalid")) ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED));
        REQUIRE(umi_build_delivery_pipeline_snapshot(owner, &after) == UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &after, sizeof before) == 0);
    }
done:
    umi_build_delivery_pipeline_destroy(owner);
    umi_build_deployment_target_registry_destroy(targets);
    umi_build_task_registry_destroy(tasks);
    return 0;
}
