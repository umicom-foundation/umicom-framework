/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/call_hierarchy_view_model.h
 *
 * PURPOSE:
 *   Aggregate call-hierarchy nodes and edges for presentation.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CALL_HIERARCHY_VIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CALL_HIERARCHY_VIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCallHierarchyViewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelCallHierarchyViewModel;
UmiStatus umi_editor_intel_call_hierarchy_view_model_init(UmiEditorIntelCallHierarchyViewModel *model);
UmiStatus umi_editor_intel_call_hierarchy_view_model_add(UmiEditorIntelCallHierarchyViewModel *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_call_hierarchy_view_model_find(const UmiEditorIntelCallHierarchyViewModel *model,const char *id);
UmiStatus umi_editor_intel_call_hierarchy_view_model_clear(UmiEditorIntelCallHierarchyViewModel *model);
int umi_editor_intel_call_hierarchy_view_model_valid(const UmiEditorIntelCallHierarchyViewModel *model);

#ifdef __cplusplus
}
#endif
#endif
