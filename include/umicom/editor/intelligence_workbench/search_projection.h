/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_projection.h
 *
 * PURPOSE:
 *   Project the canonical workspace-search orchestration into bounded result,
 *   grouping and selection state suitable for every Framework frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_PROJECTION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_PROJECTION_H

#include "umicom/editor/intelligence_workbench/search_result_group.h"
#include "umicom/editor/intelligence_workbench/projection.h"
#include "umicom/editor/intelligence_workbench/workspace_search_view_model.h"
#include "umicom/editor/workspace_search_orchestration.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_INTEL_SEARCH_PROJECTION_API_VERSION 1U

/**
 * Represent the editor intel search projection data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelSearchProjection {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorIntelWorkspaceSearchViewModel results;
    UmiEditorIntelSearchResultGroup groups;
    UmiEditorWorkspaceSearchOrchestrationSnapshot source_snapshot;
    size_t selected_index;
    uint64_t source_revision;
    uint64_t revision;
    int has_selection;
    int truncated;
} UmiEditorIntelSearchProjection;

/**
 * Initialise editor intel search projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_search_projection_init(
    UmiEditorIntelSearchProjection *projection);
/**
 * Provide the editor intel search projection refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_search_projection_refresh(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration);
/**
 * Provide the editor intel search projection select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_search_projection_select(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    size_t index);
/**
 * Provide the editor intel search projection select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_search_projection_select_next(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    int wrap);
/**
 * Provide the editor intel search projection select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_search_projection_select_previous(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    int wrap);
/**
 * Find editor intel search projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_search_projection_selected(
    const UmiEditorIntelSearchProjection *projection);
/**
 * Check that editor intel search projection satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_search_projection_valid(
    const UmiEditorIntelSearchProjection *projection);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_PROJECTION_H */
