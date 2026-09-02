/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/studio_bridge.h
 *
 * PURPOSE:
 *   Publish authoritative Studio-facing Framework model snapshots through registered structured selection providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_SELECTION_PROVIDER_STUDIO_BRIDGE_H
#define UMICOM_WORKBENCH_SELECTION_PROVIDER_STUDIO_BRIDGE_H

#include "umicom/workbench_selection_provider/problem.h"
#include "umicom/workbench_selection_provider/project.h"
#include "umicom/workbench_selection_provider/source_control.h"
#include "umicom/workbench_selection_provider/studio_profile.h"
#include "umicom/workbench_selection_provider/test.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the workbench selection provider publish project operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_publish_project(
    UmiWorkbenchSelectionProviderService *service,
    const UmiProjectWorkspaceSelectionSnapshot *snapshot,
    const char *workspace_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench selection provider publish problem operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_publish_problem(
    UmiWorkbenchSelectionProviderService *service,
    const UmiUiProblemSnapshot *problem,
    const char *workspace_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench selection provider publish source change operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_publish_source_change(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlChangeSnapshot *change,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms);

/**
 * Provide the workbench selection provider publish source commit operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_publish_source_commit(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlCommitSnapshot *commit,
    const char *workspace_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench selection provider publish source branch operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_selection_provider_publish_source_branch(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlBranchSnapshot *branch,
    const char *workspace_id,
    uint64_t timestamp_ms);

/**
 * Provide the workbench selection provider publish test row operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_publish_test_row(
    UmiWorkbenchSelectionProviderService *service,
    const UmiTestExplorerRow *row,
    const char *workspace_id,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
