/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_edit_set.h
 *
 * PURPOSE:
 *   Collect related workspace edits under one governed transaction.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel workspace edit set data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelWorkspaceEditSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceEditSet;
/**
 * Initialise editor intel workspace edit set from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workspace_edit_set_init(UmiEditorIntelWorkspaceEditSet *model);
/**
 * Add editor intel workspace edit set only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_workspace_edit_set_add(UmiEditorIntelWorkspaceEditSet *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel workspace edit set while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_workspace_edit_set_find(const UmiEditorIntelWorkspaceEditSet *model,const char *id);
/**
 * Release or reset state held by editor intel workspace edit set so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_intel_workspace_edit_set_clear(UmiEditorIntelWorkspaceEditSet *model);
/**
 * Check that editor intel workspace edit set satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_workspace_edit_set_valid(const UmiEditorIntelWorkspaceEditSet *model);

#ifdef __cplusplus
}
#endif
#endif
