/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_search_view_model.h
 *
 * PURPOSE:
 *   Aggregate grouped workspace-search results for toolkit-neutral presentation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_VIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_VIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel workspace search view model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelWorkspaceSearchViewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceSearchViewModel;
/**
 * Initialise editor intel workspace search view model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workspace_search_view_model_init(UmiEditorIntelWorkspaceSearchViewModel *model);
/**
 * Add editor intel workspace search view model only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_editor_intel_workspace_search_view_model_add(UmiEditorIntelWorkspaceSearchViewModel *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel workspace search view model while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_workspace_search_view_model_find(const UmiEditorIntelWorkspaceSearchViewModel *model,const char *id);
/**
 * Release or reset state held by editor intel workspace search view model so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_intel_workspace_search_view_model_clear(UmiEditorIntelWorkspaceSearchViewModel *model);
/**
 * Check that editor intel workspace search view model satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_workspace_search_view_model_valid(const UmiEditorIntelWorkspaceSearchViewModel *model);

#ifdef __cplusplus
}
#endif
#endif
