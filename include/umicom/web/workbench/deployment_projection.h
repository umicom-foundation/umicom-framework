/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/deployment_projection.h
 *
 * PURPOSE:
 *   Filter and project the existing Framework deployment-target registry into
 *   web/cloud tooling. Target ownership and deployment execution remain Build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_DEPLOYMENT_PROJECTION_H
#define UMICOM_WEB_WORKBENCH_DEPLOYMENT_PROJECTION_H

#include "umicom/build/deployment_target.h"
#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench deployment projection data shared with callers of this
 * public contract.
 */
typedef struct UmiWebWorkbenchDeploymentProjection {
    UmiBuildDeploymentTargetSnapshot targets[
        UMI_BUILD_DEPLOYMENT_TARGET_CAPACITY];
    size_t target_count;
    size_t production_count;
    size_t approval_count;
    size_t rollback_count;
    uint64_t source_revision;
} UmiWebWorkbenchDeploymentProjection;

/**
 * Initialise web workbench deployment projection from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_deployment_projection_init(
    UmiWebWorkbenchDeploymentProjection *projection);
/**
 * Provide the web workbench deployment projection capture operation used by this module
 * and its client applications.
 */
UmiStatus umi_web_workbench_deployment_projection_capture(
    UmiWebWorkbenchDeploymentProjection *projection,
    const UmiBuildDeploymentTargetRegistry *registry,
    const char *environment,
    bool enabled_only);
/**
 * Find web workbench deployment projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiBuildDeploymentTargetSnapshot *
umi_web_workbench_deployment_projection_find(
    const UmiWebWorkbenchDeploymentProjection *projection,
    const char *target_id);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_DEPLOYMENT_PROJECTION_H */
