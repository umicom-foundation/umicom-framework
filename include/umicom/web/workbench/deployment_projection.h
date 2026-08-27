/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/deployment_projection.h
 *
 * PURPOSE:
 *   Filter and project the existing Framework deployment-target registry into
 *   web/cloud tooling. Target ownership and deployment execution remain Build.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_DEPLOYMENT_PROJECTION_H
#define UMICOM_WEB_WORKBENCH_DEPLOYMENT_PROJECTION_H

#include "umicom/build/deployment_target.h"
#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebWorkbenchDeploymentProjection {
    UmiBuildDeploymentTargetSnapshot targets[
        UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY];
    size_t target_count;
    size_t production_count;
    size_t approval_count;
    size_t rollback_count;
    uint64_t source_revision;
} UmiWebWorkbenchDeploymentProjection;

void umi_web_workbench_deployment_projection_init(
    UmiWebWorkbenchDeploymentProjection *projection);
UmiStatus umi_web_workbench_deployment_projection_capture(
    UmiWebWorkbenchDeploymentProjection *projection,
    const UmiBuildDeploymentTargetRegistry *registry,
    const char *environment,
    bool enabled_only);
const UmiBuildDeploymentTargetSnapshot *
umi_web_workbench_deployment_projection_find(
    const UmiWebWorkbenchDeploymentProjection *projection,
    const char *target_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_DEPLOYMENT_PROJECTION_H */
