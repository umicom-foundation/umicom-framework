/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_search_snapshot.h
 *
 * PURPOSE:
 *   Model workspace search snapshot as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_SNAPSHOT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_SNAPSHOT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceSearchSnapshot { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelWorkspaceSearchSnapshot;
UmiStatus umi_editor_intel_workspace_search_snapshot_init(UmiEditorIntelWorkspaceSearchSnapshot *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_workspace_search_snapshot_set_score(UmiEditorIntelWorkspaceSearchSnapshot *model,uint32_t score);
UmiStatus umi_editor_intel_workspace_search_snapshot_set_selected(UmiEditorIntelWorkspaceSearchSnapshot *model,bool selected);
int umi_editor_intel_workspace_search_snapshot_valid(const UmiEditorIntelWorkspaceSearchSnapshot *model);

#ifdef __cplusplus
}
#endif
#endif
