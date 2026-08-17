/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_pipeline.c
 *
 * PURPOSE:
 *   Verify one complete task, artifact and deployment vertical slice.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/pipeline.h"

int main(void)
{
    UmiBuildTaskRegistry *tasks = NULL;
    UmiBuildDeploymentTargetRegistry *targets = NULL;
    UmiBuildDeliveryPipeline *pipeline = NULL;
    UmiBuildTaskSnapshot build;
    UmiBuildTaskSnapshot ready;
    UmiBuildArtifactManifest artifact;
    UmiBuildDeploymentTargetSnapshot target;
    UmiBuildDeploymentStepSnapshot step;
    UmiBuildDeliveryPipelineSnapshot snapshot;

    assert(umi_build_task_registry_create(&tasks) == UMI_STATUS_OK);
    umi_build_task_init(&build, "build", "Build", UMI_BUILD_TASK_BUILD);
    assert(umi_build_task_set_command(
               &build, "cmake --build build", ".") == UMI_STATUS_OK);
    assert(umi_build_task_registry_upsert(tasks, &build) == UMI_STATUS_OK);

    assert(umi_build_deployment_target_registry_create(&targets) ==
           UMI_STATUS_OK);
    umi_build_deployment_target_init(
        &target, "local", "Local Install",
        UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY);
    strcpy(target.destination, "install/studio");
    assert(umi_build_deployment_target_registry_upsert(targets, &target) ==
           UMI_STATUS_OK);

    assert(umi_build_delivery_pipeline_create(
               "studio.delivery", tasks, targets, 1U, &pipeline) ==
           UMI_STATUS_OK);
    assert(umi_build_delivery_pipeline_plan_tasks(
               pipeline, "build.operation", "build") == UMI_STATUS_OK);
    assert(umi_build_delivery_pipeline_begin_tasks(pipeline) == UMI_STATUS_OK);
    assert(umi_build_delivery_pipeline_next_task(pipeline, &ready) ==
           UMI_STATUS_OK);
    assert(strcmp(ready.task_id, "build") == 0);
    assert(umi_build_delivery_pipeline_start_task(
               pipeline, ready.task_id) == UMI_STATUS_OK);
    assert(umi_build_delivery_pipeline_complete_task(
               pipeline, ready.task_id, 0) == UMI_STATUS_OK);

    umi_build_artifact_manifest_init(
        &artifact, "studio.package", "dist/studio.zip", "archive");
    artifact.flags |= UMI_BUILD_ARTIFACT_PRIMARY |
                      UMI_BUILD_ARTIFACT_DEPLOYABLE |
                      UMI_BUILD_ARTIFACT_VERIFIED;
    strcpy(artifact.checksum_algorithm, "SHA-256");
    strcpy(artifact.checksum, "abc");
    assert(umi_build_delivery_pipeline_register_artifact(
               pipeline, &artifact) == UMI_STATUS_OK);
    assert(umi_build_delivery_pipeline_plan_deployment(
               pipeline, "deploy.local", "studio.package", "local", 1) ==
           UMI_STATUS_OK);
    assert(umi_build_delivery_pipeline_begin_deployment(pipeline) ==
           UMI_STATUS_OK);
    while (umi_build_delivery_pipeline_next_deployment_step(
               pipeline, &step) == UMI_STATUS_OK) {
        if (step.rollback_step) break;
        assert(umi_build_delivery_pipeline_start_deployment_step(
                   pipeline, step.step_id) == UMI_STATUS_OK);
        assert(umi_build_delivery_pipeline_complete_deployment_step(
                   pipeline, step.step_id, 0) == UMI_STATUS_OK);
    }
    assert(umi_build_delivery_pipeline_snapshot(pipeline, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.phase == UMI_BUILD_PIPELINE_SUCCEEDED);
    assert(snapshot.artifacts.artifact_count == 1U);
    assert(snapshot.deployment.state == UMI_BUILD_DEPLOYMENT_SUCCEEDED);

    umi_build_delivery_pipeline_destroy(pipeline);
    umi_build_deployment_target_registry_destroy(targets);
    umi_build_task_registry_destroy(tasks);
    return 0;
}
