/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_deployment_projection.c
 * PURPOSE: Verify read-only projection of canonical web deployment targets.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/web/workbench/deployment_projection.h"

int main(void)
{
    UmiBuildDeploymentTargetRegistry *registry = NULL;
    UmiBuildDeploymentTargetSnapshot target;
    UmiWebWorkbenchDeploymentProjection *projection = calloc(1U, sizeof(*projection));
    assert(projection != NULL);
    assert(umi_build_deployment_target_registry_create(&registry) == UMI_STATUS_OK);
    umi_build_deployment_target_init(&target, "web-prod", "Web Production",
        UMI_BUILD_DEPLOYMENT_STATIC_SITE);
    assert(umi_web_workbench_copy_text(target.environment,
        sizeof(target.environment), "production") == UMI_STATUS_OK);
    assert(umi_web_workbench_copy_text(target.destination,
        sizeof(target.destination), "s3://web-production") == UMI_STATUS_OK);
    target.flags = UMI_BUILD_DEPLOYMENT_TARGET_ENABLED |
        UMI_BUILD_DEPLOYMENT_TARGET_PRODUCTION |
        UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL |
        UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK;
    assert(umi_build_deployment_target_registry_upsert(registry, &target) == UMI_STATUS_OK);
    assert(umi_web_workbench_deployment_projection_capture(projection,
        registry, "production", true) == UMI_STATUS_OK);
    assert(projection->target_count == 1U && projection->approval_count == 1U);
    assert(umi_web_workbench_deployment_projection_find(projection,
        "web-prod") != NULL);
    umi_build_deployment_target_registry_destroy(registry);
    free(projection);
    return 0;
}
