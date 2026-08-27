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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_VIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_VIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceSearchViewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceSearchViewModel;
UmiStatus umi_editor_intel_workspace_search_view_model_init(UmiEditorIntelWorkspaceSearchViewModel *model);
UmiStatus umi_editor_intel_workspace_search_view_model_add(UmiEditorIntelWorkspaceSearchViewModel *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_workspace_search_view_model_find(const UmiEditorIntelWorkspaceSearchViewModel *model,const char *id);
UmiStatus umi_editor_intel_workspace_search_view_model_clear(UmiEditorIntelWorkspaceSearchViewModel *model);
int umi_editor_intel_workspace_search_view_model_valid(const UmiEditorIntelWorkspaceSearchViewModel *model);

#ifdef __cplusplus
}
#endif
#endif
