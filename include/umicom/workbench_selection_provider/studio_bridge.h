/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_selection_provider/studio_bridge.h
 *
 * PURPOSE:
 *   Publish authoritative Studio-facing Framework model snapshots through registered structured selection providers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_workbench_selection_provider_publish_project(
    UmiWorkbenchSelectionProviderService *service,
    const UmiProjectWorkspaceSelectionSnapshot *snapshot,
    const char *workspace_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_provider_publish_problem(
    UmiWorkbenchSelectionProviderService *service,
    const UmiUiProblemSnapshot *problem,
    const char *workspace_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_provider_publish_source_change(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlChangeSnapshot *change,
    const char *workspace_id,
    const char *workspace_root,
    const char *branch,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_provider_publish_source_commit(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlCommitSnapshot *commit,
    const char *workspace_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_provider_publish_source_branch(
    UmiWorkbenchSelectionProviderService *service,
    const UmiSourceControlBranchSnapshot *branch,
    const char *workspace_id,
    uint64_t timestamp_ms);

UmiStatus umi_workbench_selection_provider_publish_test_row(
    UmiWorkbenchSelectionProviderService *service,
    const UmiTestExplorerRow *row,
    const char *workspace_id,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
