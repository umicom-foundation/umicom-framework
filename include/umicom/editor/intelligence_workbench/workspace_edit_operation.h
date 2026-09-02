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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_OPERATION_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_OPERATION_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel workspace edit operation data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelWorkspaceEditOperation { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelWorkspaceEditOperation;
/**
 * Initialise editor intel workspace edit operation from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workspace_edit_operation_init(UmiEditorIntelWorkspaceEditOperation *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel workspace edit operation set score operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_workspace_edit_operation_set_score(UmiEditorIntelWorkspaceEditOperation *model,uint32_t score);
/**
 * Find editor intel workspace edit operation set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_intel_workspace_edit_operation_set_selected(UmiEditorIntelWorkspaceEditOperation *model,bool selected);
/**
 * Check that editor intel workspace edit operation satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_workspace_edit_operation_valid(const UmiEditorIntelWorkspaceEditOperation *model);

#ifdef __cplusplus
}
#endif
#endif
