/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_deployment_target.c
 *
 * PURPOSE:
 *   Verify deployment-target safety gates and registry classifications.
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

#include "umicom/build/deployment_target.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiBuildDeploymentTargetRegistry *registry = NULL;
    UmiBuildDeploymentTargetSnapshot target;
    UmiBuildDeploymentTargetSnapshot invalid;
    UmiBuildDeploymentTargetRegistrySnapshot snapshot;
    char message[256];

    umi_build_deployment_target_init(
        &target, "production", "Production Service",
        UMI_BUILD_DEPLOYMENT_SERVICE);
    strcpy(target.environment, "production");
    strcpy(target.destination, "C:/Apps/UmicomStudio");
    strcpy(target.working_directory, "C:/Apps/UmicomStudio");
    strcpy(target.health_uri, "http://127.0.0.1:8080/health");
    target.strategy = UMI_BUILD_DEPLOYMENT_BLUE_GREEN;
    target.flags |= UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_TRUST |
                    UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL |
                    UMI_BUILD_DEPLOYMENT_TARGET_HEALTH_CHECK |
                    UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK |
                    UMI_BUILD_DEPLOYMENT_TARGET_PRODUCTION;
    assert(umi_build_deployment_target_validate(
               &target, message, sizeof(message)) == UMI_STATUS_OK);
    assert(umi_build_deployment_target_registry_create(&registry) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_target_registry_upsert(registry, &target) ==
           UMI_STATUS_OK);
    assert(umi_build_deployment_target_registry_snapshot(
               registry, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.production_count == 1U);
    assert(snapshot.approval_count == 1U);
    assert(snapshot.rollback_count == 1U);

    umi_build_deployment_target_init(
        &invalid, "remote", "Remote", UMI_BUILD_DEPLOYMENT_REMOTE_HOST);
    strcpy(invalid.destination, "/opt/umicom");
    invalid.flags |= UMI_BUILD_DEPLOYMENT_TARGET_REMOTE;
    assert(umi_build_deployment_target_validate(
               &invalid, message, sizeof(message)) ==
           UMI_STATUS_INVALID_ARGUMENT);

    umi_build_deployment_target_registry_destroy(registry);
    return 0;
}
