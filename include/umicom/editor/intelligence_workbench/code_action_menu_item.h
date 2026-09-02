/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/code_action_menu_item.h
 *
 * PURPOSE:
 *   Model code action menu item as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel code action menu item data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelCodeActionMenuItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionMenuItem;
/**
 * Initialise editor intel code action menu item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_menu_item_init(UmiEditorIntelCodeActionMenuItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel code action menu item set score operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_action_menu_item_set_score(UmiEditorIntelCodeActionMenuItem *model,uint32_t score);
/**
 * Find editor intel code action menu item set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_code_action_menu_item_set_selected(UmiEditorIntelCodeActionMenuItem *model,bool selected);
/**
 * Check that editor intel code action menu item satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_action_menu_item_valid(const UmiEditorIntelCodeActionMenuItem *model);

#ifdef __cplusplus
}
#endif
#endif
