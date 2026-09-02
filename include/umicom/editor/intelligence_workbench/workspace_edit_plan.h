/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_edit_plan.h
 *
 * PURPOSE:
 *   Build a deterministic, reviewable workspace edit plan.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_PLAN_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_PLAN_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel workspace edit plan data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorIntelWorkspaceEditPlan { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceEditPlan;
/**
 * Initialise editor intel workspace edit plan from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_workspace_edit_plan_init(UmiEditorIntelWorkspaceEditPlan *model);
/**
 * Add editor intel workspace edit plan only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_intel_workspace_edit_plan_add(UmiEditorIntelWorkspaceEditPlan *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel workspace edit plan while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_workspace_edit_plan_find(const UmiEditorIntelWorkspaceEditPlan *model,const char *id);
/**
 * Release or reset state held by editor intel workspace edit plan so the same storage can
 * be reused safely.
 */
UmiStatus umi_editor_intel_workspace_edit_plan_clear(UmiEditorIntelWorkspaceEditPlan *model);
/**
 * Check that editor intel workspace edit plan satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_workspace_edit_plan_valid(const UmiEditorIntelWorkspaceEditPlan *model);

#ifdef __cplusplus
}
#endif
#endif
