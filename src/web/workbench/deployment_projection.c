/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/deployment_projection.c
 *
 * PURPOSE:
 *   Implement read-only filtering of canonical Framework deployment targets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/deployment_projection.h"

#include <string.h>

/*
 * Provide the is web cloud target operation used by this module and its client
 * applications.
 */
static bool is_web_cloud_target(UmiBuildDeploymentTargetKind kind)
{
    return kind == UMI_BUILD_DEPLOYMENT_SERVICE ||
           kind == UMI_BUILD_DEPLOYMENT_CONTAINER ||
           kind == UMI_BUILD_DEPLOYMENT_REMOTE_HOST ||
           kind == UMI_BUILD_DEPLOYMENT_STATIC_SITE ||
           kind == UMI_BUILD_DEPLOYMENT_PACKAGE_REPOSITORY;
}

/*
 * Initialise web workbench deployment projection from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_deployment_projection_init(
    UmiWebWorkbenchDeploymentProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return;
    memset(projection, 0, sizeof(*projection));
}

/*
 * Provide the web workbench deployment projection capture operation used by this module
 * and its client applications.
 */
UmiStatus umi_web_workbench_deployment_projection_capture(
    UmiWebWorkbenchDeploymentProjection *projection,
    const UmiBuildDeploymentTargetRegistry *registry,
    const char *environment,
    bool enabled_only)
{
    size_t index;
    size_t source_count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_web_workbench_deployment_projection_init(projection);
    source_count = umi_build_deployment_target_registry_count(registry);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < source_count; ++index) {
        UmiBuildDeploymentTargetSnapshot target;
        UmiStatus status = umi_build_deployment_target_registry_at(
            registry, index, &target);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!is_web_cloud_target(target.kind)) continue;
        /* Create this optional product surface only when its build option is enabled. */
        if (enabled_only && (target.flags & UMI_BUILD_DEPLOYMENT_TARGET_ENABLED) == 0U) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (environment != NULL && environment[0] != '\0' &&
            strcmp(target.environment, environment) != 0) continue;
        /* Create this optional product surface only when its build option is enabled. */
        if (projection->target_count >= UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        projection->targets[projection->target_count++] = target;
        /* Create this optional product surface only when its build option is enabled. */
        if ((target.flags & UMI_BUILD_DEPLOYMENT_TARGET_PRODUCTION) != 0U) {
            projection->production_count++;
        }
        /* Create this optional product surface only when its build option is enabled. */
        if ((target.flags & UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL) != 0U) {
            projection->approval_count++;
        }
        /* Create this optional product surface only when its build option is enabled. */
        if ((target.flags & UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK) != 0U) {
            projection->rollback_count++;
        }
    }
    projection->source_revision =
        umi_build_deployment_target_registry_revision(registry);
    return UMI_STATUS_OK;
}

/*
 * Find web workbench deployment projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiBuildDeploymentTargetSnapshot *
umi_web_workbench_deployment_projection_find(
    const UmiWebWorkbenchDeploymentProjection *projection,
    const char *target_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || target_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < projection->target_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(projection->targets[index].target_id, target_id) == 0) {
            return &projection->targets[index];
        }
    }
    return NULL;
}
