/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_symbol_result.h
 *
 * PURPOSE:
 *   Model workspace symbol result as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_RESULT_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_RESULT_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceSymbolResult { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelWorkspaceSymbolResult;
UmiStatus umi_editor_intel_workspace_symbol_result_init(UmiEditorIntelWorkspaceSymbolResult *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_workspace_symbol_result_set_score(UmiEditorIntelWorkspaceSymbolResult *model,uint32_t score);
UmiStatus umi_editor_intel_workspace_symbol_result_set_selected(UmiEditorIntelWorkspaceSymbolResult *model,bool selected);
int umi_editor_intel_workspace_symbol_result_valid(const UmiEditorIntelWorkspaceSymbolResult *model);

#ifdef __cplusplus
}
#endif
#endif
