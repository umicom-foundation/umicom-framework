/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/source_control.h
 *
 * PURPOSE:
 *   Adapt provider-neutral Source Control change, commit and branch snapshots into structured selections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_SOURCE_CONTROL_H

#include "umicom/source_control/change.h"
#include "umicom/source_control/commit.h"
#include "umicom/source_control/branch.h"
#include "umicom/workbench_selection_provider/identity.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the workbench selection provider from source control change operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_from_source_control_change(
    const UmiSourceControlChangeSnapshot *change,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

/**
 * Provide the workbench selection provider from source control commit operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_from_source_control_commit(
    const UmiSourceControlCommitSnapshot *commit,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

/**
 * Provide the workbench selection provider from source control branch operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_from_source_control_branch(
    const UmiSourceControlBranchSnapshot *branch,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

/**
 * Find workbench selection provider source control change while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_source_control_change_find(
    const UmiSourceControlChangeRegistry *registry,
    const char *change_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

/**
 * Find workbench selection provider source control commit while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_source_control_commit_find(
    const UmiSourceControlCommitRegistry *registry,
    const char *commit_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

/**
 * Find workbench selection provider source control branch while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_workbench_selection_provider_source_control_branch_find(
    const UmiSourceControlBranchRegistry *registry,
    const char *branch_id,
    const char *application_id,
    const char *panel_id,
    const char *workspace_id,
    uint64_t timestamp_ms,
    UmiWorkbenchSelection *out_selection);

#ifdef __cplusplus
}
#endif
#endif
