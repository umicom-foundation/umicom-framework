/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_symbol_group.h
 *
 * PURPOSE:
 *   Model workspace symbol group as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel workspace symbol group data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelWorkspaceSymbolGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceSymbolGroup;
/**
 * Initialise editor intel workspace symbol group from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workspace_symbol_group_init(UmiEditorIntelWorkspaceSymbolGroup *model);
/**
 * Add editor intel workspace symbol group only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_workspace_symbol_group_add(UmiEditorIntelWorkspaceSymbolGroup *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel workspace symbol group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_workspace_symbol_group_find(const UmiEditorIntelWorkspaceSymbolGroup *model,const char *id);
/**
 * Release or reset state held by editor intel workspace symbol group so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_workspace_symbol_group_clear(UmiEditorIntelWorkspaceSymbolGroup *model);
/**
 * Check that editor intel workspace symbol group satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_workspace_symbol_group_valid(const UmiEditorIntelWorkspaceSymbolGroup *model);

#ifdef __cplusplus
}
#endif
#endif
