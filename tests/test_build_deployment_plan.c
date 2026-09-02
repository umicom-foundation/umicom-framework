/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_deployment_plan.c
 *
 * PURPOSE:
 *   Verify safe default deployment composition, approval and rollback planning.
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

#include "umicom/build/deployment_plan.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildArtifactManifest artifact;
    UmiBuildDeploymentTargetSnapshot target;
    UmiBuildDeploymentPlan plan;
    UmiBuildDeploymentPlanSnapshot snapshot;
    UmiBuildDeploymentStepSnapshot step;
    size_t index;
    int transfer_found = 0;
    int rollback_found = 0;
    char message[256];

    umi_build_artifact_manifest_init(
        &artifact, "studio.package", "dist/studio.zip", "archive");
    artifact.flags |= UMI_BUILD_ARTIFACT_DEPLOYABLE |
                      UMI_BUILD_ARTIFACT_VERIFIED;
    strcpy(artifact.checksum_algorithm, "SHA-256");
    strcpy(artifact.checksum, "0123456789");

    umi_build_deployment_target_init(
        &target, "remote-production", "Remote Production",
        UMI_BUILD_DEPLOYMENT_REMOTE_HOST);
    strcpy(target.environment, "production");
    strcpy(target.destination, "/opt/umicom/studio");
    strcpy(target.endpoint, "build-host.example");
    strcpy(target.health_uri, "https://studio.example/health");
    target.flags |= UMI_BUILD_DEPLOYMENT_TARGET_REMOTE |
                    UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL |
                    UMI_BUILD_DEPLOYMENT_TARGET_HEALTH_CHECK |
                    UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK;

    umi_build_deployment_plan_init(&plan, "deploy.production");
    assert(umi_build_deployment_plan_compose_default(
               &plan, &artifact, &target, 1) == UMI_STATUS_OK);
    assert(umi_build_deployment_plan_validate(
               &plan, message, sizeof(message)) == UMI_STATUS_OK);
    assert(plan.requires_approval != 0);
    assert(plan.approved == 0);
    assert(umi_build_deployment_plan_approve(&plan) == UMI_STATUS_OK);
    assert(umi_build_deployment_plan_snapshot(&plan, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.valid != 0);
    assert(snapshot.approved != 0);
    assert(snapshot.rollback_step_count == 1U);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan.step_count; ++index) {
        assert(umi_build_deployment_plan_step_at(&plan, index, &step) ==
               UMI_STATUS_OK);
        /* Create this optional product surface only when its build option is enabled. */
        if (step.kind == UMI_BUILD_DEPLOYMENT_STEP_TRANSFER)
            transfer_found = 1;
        /* Create this optional product surface only when its build option is enabled. */
        if (step.kind == UMI_BUILD_DEPLOYMENT_STEP_ROLLBACK)
            rollback_found = 1;
    }
    assert(transfer_found != 0);
    assert(rollback_found != 0);
    return 0;
}
