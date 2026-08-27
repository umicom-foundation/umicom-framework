/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/search_projection.h
 *
 * PURPOSE:
 *   Project the canonical workspace-search orchestration into bounded result,
 *   grouping and selection state suitable for every Framework frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

UmiStatus umi_editor_intel_search_projection_init(
    UmiEditorIntelSearchProjection *projection);
UmiStatus umi_editor_intel_search_projection_refresh(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration);
UmiStatus umi_editor_intel_search_projection_select(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    size_t index);
UmiStatus umi_editor_intel_search_projection_select_next(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    int wrap);
UmiStatus umi_editor_intel_search_projection_select_previous(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    int wrap);
const UmiEditorIntelEntry *umi_editor_intel_search_projection_selected(
    const UmiEditorIntelSearchProjection *projection);
int umi_editor_intel_search_projection_valid(
    const UmiEditorIntelSearchProjection *projection);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_SEARCH_PROJECTION_H */
