/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/project/workspace_refresh.h
 * PURPOSE: Plan deterministic, non-destructive workspace import refreshes.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROJECT_WORKSPACE_REFRESH_H
#define UMICOM_PROJECT_WORKSPACE_REFRESH_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/project/workspace_discovery.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROJECT_WORKSPACE_REFRESH_API_VERSION 1U

/**
 * Represent the project workspace refresh snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiProjectWorkspaceRefreshSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t source_revision;
    UmiProjectWorkspaceDiscoverySnapshot discovery;
    size_t existing_project_count;
    size_t unchanged_project_count;
    size_t import_candidate_count;
    size_t missing_project_count;
    int requires_confirmation;
} UmiProjectWorkspaceRefreshSnapshot;

/**
 * Provide the project workspace model plan refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_project_workspace_model_plan_refresh(
    const UmiProjectWorkspaceModel *model,
    const UmiProjectWorkspaceDiscoveryOptions *options,
    UmiProjectWorkspaceRefreshSnapshot *out_refresh);

#ifdef __cplusplus
}
#endif
#endif
