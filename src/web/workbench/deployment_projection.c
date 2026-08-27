/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/deployment_projection.c
 *
 * PURPOSE:
 *   Implement read-only filtering of canonical Framework deployment targets.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/deployment_projection.h"

#include <string.h>

static bool is_web_cloud_target(UmiBuildDeploymentTargetKind kind)
{
    return kind == UMI_BUILD_DEPLOYMENT_SERVICE ||
           kind == UMI_BUILD_DEPLOYMENT_CONTAINER ||
           kind == UMI_BUILD_DEPLOYMENT_REMOTE_HOST ||
           kind == UMI_BUILD_DEPLOYMENT_STATIC_SITE ||
           kind == UMI_BUILD_DEPLOYMENT_PACKAGE_REPOSITORY;
}

void umi_web_workbench_deployment_projection_init(
    UmiWebWorkbenchDeploymentProjection *projection)
{
    if (projection == NULL) return;
    memset(projection, 0, sizeof(*projection));
}

UmiStatus umi_web_workbench_deployment_projection_capture(
    UmiWebWorkbenchDeploymentProjection *projection,
    const UmiBuildDeploymentTargetRegistry *registry,
    const char *environment,
    bool enabled_only)
{
    size_t index;
    size_t source_count;
    if (projection == NULL || registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_web_workbench_deployment_projection_init(projection);
    source_count = umi_build_deployment_target_registry_count(registry);
    for (index = 0U; index < source_count; ++index) {
        UmiBuildDeploymentTargetSnapshot target;
        UmiStatus status = umi_build_deployment_target_registry_at(
            registry, index, &target);
        if (status != UMI_STATUS_OK) return status;
        if (!is_web_cloud_target(target.kind)) continue;
        if (enabled_only && (target.flags & UMI_BUILD_DEPLOYMENT_TARGET_ENABLED) == 0U) continue;
        if (environment != NULL && environment[0] != '\0' &&
            strcmp(target.environment, environment) != 0) continue;
        if (projection->target_count >= UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        projection->targets[projection->target_count++] = target;
        if ((target.flags & UMI_BUILD_DEPLOYMENT_TARGET_PRODUCTION) != 0U) {
            projection->production_count++;
        }
        if ((target.flags & UMI_BUILD_DEPLOYMENT_TARGET_REQUIRES_APPROVAL) != 0U) {
            projection->approval_count++;
        }
        if ((target.flags & UMI_BUILD_DEPLOYMENT_TARGET_SUPPORTS_ROLLBACK) != 0U) {
            projection->rollback_count++;
        }
    }
    projection->source_revision =
        umi_build_deployment_target_registry_revision(registry);
    return UMI_STATUS_OK;
}

const UmiBuildDeploymentTargetSnapshot *
umi_web_workbench_deployment_projection_find(
    const UmiWebWorkbenchDeploymentProjection *projection,
    const char *target_id)
{
    size_t index;
    if (projection == NULL || target_id == NULL) return NULL;
    for (index = 0U; index < projection->target_count; ++index) {
        if (strcmp(projection->targets[index].target_id, target_id) == 0) {
            return &projection->targets[index];
        }
    }
    return NULL;
}
