/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_deployment_controller.c
 *
 * PURPOSE:
 *   Verify ordered deployment execution, failure evidence and rollback state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/build/deployment_controller.h"

static void make_plan(UmiBuildDeploymentPlan *plan)
{
    UmiBuildArtifactManifest artifact;
    UmiBuildDeploymentTargetSnapshot target;
    umi_build_artifact_manifest_init(
        &artifact, "studio", "dist/studio.zip", "archive");
    artifact.flags |= UMI_BUILD_ARTIFACT_DEPLOYABLE;
    umi_build_deployment_target_init(
        &target, "local", "Local", UMI_BUILD_DEPLOYMENT_LOCAL_DIRECTORY);
    strcpy(target.destination, "install/studio");
    target.flags |= UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK;
    umi_build_deployment_plan_init(plan, "deploy.local");
    assert(umi_build_deployment_plan_compose_default(
               plan, &artifact, &target, 0) == UMI_STATUS_OK);
}

int main(void)
{
    UmiBuildDeploymentController *controller = NULL;
    UmiBuildDeploymentControllerSnapshot snapshot;
    UmiBuildDeploymentPlan plan;
    UmiBuildDeploymentStepSnapshot step;
    size_t completed = 0U;

    make_plan(&plan);
    assert(umi_build_deployment_controller_create(&controller) == UMI_STATUS_OK);
    assert(umi_build_deployment_controller_load(controller, &plan) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_controller_begin(controller) == UMI_STATUS_OK);
    while (umi_build_deployment_controller_next_step(controller, &step) ==
           UMI_STATUS_OK) {
        if (step.rollback_step) break;
        assert(umi_build_deployment_controller_start_step(
                   controller, step.step_id) == UMI_STATUS_OK);
        assert(umi_build_deployment_controller_complete_step(
                   controller, step.step_id, 0) == UMI_STATUS_OK);
        completed += 1U;
    }
    assert(completed >= 6U);
    assert(umi_build_deployment_controller_snapshot(controller, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_BUILD_DEPLOYMENT_SUCCEEDED);

    make_plan(&plan);
    assert(umi_build_deployment_controller_load(controller, &plan) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_controller_begin(controller) == UMI_STATUS_OK);
    assert(umi_build_deployment_controller_next_step(controller, &step) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_controller_start_step(
               controller, step.step_id) == UMI_STATUS_OK);
    assert(umi_build_deployment_controller_fail_step(
               controller, step.step_id, UMI_STATUS_IO_ERROR, 5,
               "simulated failure") == UMI_STATUS_OK);
    assert(umi_build_deployment_controller_begin_rollback(controller) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_controller_next_step(controller, &step) ==
           UMI_STATUS_OK);
    assert(step.rollback_step != 0);
    assert(umi_build_deployment_controller_start_step(
               controller, step.step_id) == UMI_STATUS_OK);
    assert(umi_build_deployment_controller_complete_step(
               controller, step.step_id, 0) == UMI_STATUS_OK);
    assert(umi_build_deployment_controller_complete_rollback(controller, 0) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_controller_snapshot(controller, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.state == UMI_BUILD_DEPLOYMENT_ROLLED_BACK);

    umi_build_deployment_controller_destroy(controller);
    return 0;
}
