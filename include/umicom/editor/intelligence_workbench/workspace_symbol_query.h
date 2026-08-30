/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_symbol_query.h
 *
 * PURPOSE:
 *   Represent a cross-workspace symbol lookup request.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_QUERY_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_SYMBOL_QUERY_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceSymbolQuery { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelWorkspaceSymbolQuery;
UmiStatus umi_editor_intel_workspace_symbol_query_init(UmiEditorIntelWorkspaceSymbolQuery *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_workspace_symbol_query_set_score(UmiEditorIntelWorkspaceSymbolQuery *model,uint32_t score);
UmiStatus umi_editor_intel_workspace_symbol_query_set_selected(UmiEditorIntelWorkspaceSymbolQuery *model,bool selected);
int umi_editor_intel_workspace_symbol_query_valid(const UmiEditorIntelWorkspaceSymbolQuery *model);

#ifdef __cplusplus
}
#endif
#endif
