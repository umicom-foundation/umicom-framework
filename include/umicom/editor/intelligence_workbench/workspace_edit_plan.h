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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_PLAN_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_PLAN_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceEditPlan { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceEditPlan;
UmiStatus umi_editor_intel_workspace_edit_plan_init(UmiEditorIntelWorkspaceEditPlan *model);
UmiStatus umi_editor_intel_workspace_edit_plan_add(UmiEditorIntelWorkspaceEditPlan *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_workspace_edit_plan_find(const UmiEditorIntelWorkspaceEditPlan *model,const char *id);
UmiStatus umi_editor_intel_workspace_edit_plan_clear(UmiEditorIntelWorkspaceEditPlan *model);
int umi_editor_intel_workspace_edit_plan_valid(const UmiEditorIntelWorkspaceEditPlan *model);

#ifdef __cplusplus
}
#endif
#endif
