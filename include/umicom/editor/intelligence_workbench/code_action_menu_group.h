/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_menu_group.h
 *
 * PURPOSE:
 *   Group applicable code actions into a deterministic contextual menu.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code action menu group data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelCodeActionMenuGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelCodeActionMenuGroup;
/**
 * Initialise editor intel code action menu group from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_menu_group_init(UmiEditorIntelCodeActionMenuGroup *model);
/**
 * Add editor intel code action menu group only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_code_action_menu_group_add(UmiEditorIntelCodeActionMenuGroup *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel code action menu group while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_code_action_menu_group_find(const UmiEditorIntelCodeActionMenuGroup *model,const char *id);
/**
 * Release or reset state held by editor intel code action menu group so the same storage
 * can be reused safely.
 */
UmiStatus umi_editor_intel_code_action_menu_group_clear(UmiEditorIntelCodeActionMenuGroup *model);
/**
 * Check that editor intel code action menu group satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_action_menu_group_valid(const UmiEditorIntelCodeActionMenuGroup *model);

#ifdef __cplusplus
}
#endif
#endif
