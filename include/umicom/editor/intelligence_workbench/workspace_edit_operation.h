/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_edit_operation.h
 *
 * PURPOSE:
 *   Model workspace edit operation as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_OPERATION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_OPERATION_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceEditOperation { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelWorkspaceEditOperation;
UmiStatus umi_editor_intel_workspace_edit_operation_init(UmiEditorIntelWorkspaceEditOperation *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_workspace_edit_operation_set_score(UmiEditorIntelWorkspaceEditOperation *model,uint32_t score);
UmiStatus umi_editor_intel_workspace_edit_operation_set_selected(UmiEditorIntelWorkspaceEditOperation *model,bool selected);
int umi_editor_intel_workspace_edit_operation_valid(const UmiEditorIntelWorkspaceEditOperation *model);

#ifdef __cplusplus
}
#endif
#endif
