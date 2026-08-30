/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/workspace_edit_set.h
 *
 * PURPOSE:
 *   Collect related workspace edits under one governed transaction.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_SET_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_WORKSPACE_EDIT_SET_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelWorkspaceEditSet { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelWorkspaceEditSet;
UmiStatus umi_editor_intel_workspace_edit_set_init(UmiEditorIntelWorkspaceEditSet *model);
UmiStatus umi_editor_intel_workspace_edit_set_add(UmiEditorIntelWorkspaceEditSet *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_workspace_edit_set_find(const UmiEditorIntelWorkspaceEditSet *model,const char *id);
UmiStatus umi_editor_intel_workspace_edit_set_clear(UmiEditorIntelWorkspaceEditSet *model);
int umi_editor_intel_workspace_edit_set_valid(const UmiEditorIntelWorkspaceEditSet *model);

#ifdef __cplusplus
}
#endif
#endif
