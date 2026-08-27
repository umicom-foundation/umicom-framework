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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_ITEM_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_ITEM_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionMenuItem { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCodeActionMenuItem;
UmiStatus umi_editor_intel_code_action_menu_item_init(UmiEditorIntelCodeActionMenuItem *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_code_action_menu_item_set_score(UmiEditorIntelCodeActionMenuItem *model,uint32_t score);
UmiStatus umi_editor_intel_code_action_menu_item_set_selected(UmiEditorIntelCodeActionMenuItem *model,bool selected);
int umi_editor_intel_code_action_menu_item_valid(const UmiEditorIntelCodeActionMenuItem *model);

#ifdef __cplusplus
}
#endif
#endif
