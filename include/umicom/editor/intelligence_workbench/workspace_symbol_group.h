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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_GROUP_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_GROUP_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceSymbolGroup { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceSymbolGroup;
UmiStatus umi_editor_intel_workspace_symbol_group_init(UmiEditorIntelWorkspaceSymbolGroup *model);
UmiStatus umi_editor_intel_workspace_symbol_group_add(UmiEditorIntelWorkspaceSymbolGroup *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_workspace_symbol_group_find(const UmiEditorIntelWorkspaceSymbolGroup *model,const char *id);
UmiStatus umi_editor_intel_workspace_symbol_group_clear(UmiEditorIntelWorkspaceSymbolGroup *model);
int umi_editor_intel_workspace_symbol_group_valid(const UmiEditorIntelWorkspaceSymbolGroup *model);

#ifdef __cplusplus
}
#endif
#endif
