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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_SNAPSHOT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SEARCH_SNAPSHOT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel workspace search snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelWorkspaceSearchSnapshot { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelWorkspaceSearchSnapshot;
/**
 * Initialise editor intel workspace search snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workspace_search_snapshot_init(UmiEditorIntelWorkspaceSearchSnapshot *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel workspace search snapshot set score operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_workspace_search_snapshot_set_score(UmiEditorIntelWorkspaceSearchSnapshot *model,uint32_t score);
/**
 * Find editor intel workspace search snapshot set while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_intel_workspace_search_snapshot_set_selected(UmiEditorIntelWorkspaceSearchSnapshot *model,bool selected);
/**
 * Check that editor intel workspace search snapshot satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_workspace_search_snapshot_valid(const UmiEditorIntelWorkspaceSearchSnapshot *model);

#ifdef __cplusplus
}
#endif
#endif
