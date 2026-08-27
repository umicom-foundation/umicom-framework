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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CODE_ACTION_MENU_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCodeActionMenuGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelCodeActionMenuGroup;
UmiStatus umi_editor_intel_code_action_menu_group_init(UmiEditorIntelCodeActionMenuGroup *model);
UmiStatus umi_editor_intel_code_action_menu_group_add(UmiEditorIntelCodeActionMenuGroup *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_code_action_menu_group_find(const UmiEditorIntelCodeActionMenuGroup *model,const char *id);
UmiStatus umi_editor_intel_code_action_menu_group_clear(UmiEditorIntelCodeActionMenuGroup *model);
int umi_editor_intel_code_action_menu_group_valid(const UmiEditorIntelCodeActionMenuGroup *model);

#ifdef __cplusplus
}
#endif
#endif
