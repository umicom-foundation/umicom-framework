/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/type_hierarchy_view_model.h
 *
 * PURPOSE:
 *   Aggregate type-hierarchy nodes and edges for presentation.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_TYPE_HIERARCHY_VIEW_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_TYPE_HIERARCHY_VIEW_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelTypeHierarchyViewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelTypeHierarchyViewModel;
UmiStatus umi_editor_intel_type_hierarchy_view_model_init(UmiEditorIntelTypeHierarchyViewModel *model);
UmiStatus umi_editor_intel_type_hierarchy_view_model_add(UmiEditorIntelTypeHierarchyViewModel *model,const UmiEditorIntelEntry *entry);
const UmiEditorIntelEntry *umi_editor_intel_type_hierarchy_view_model_find(const UmiEditorIntelTypeHierarchyViewModel *model,const char *id);
UmiStatus umi_editor_intel_type_hierarchy_view_model_clear(UmiEditorIntelTypeHierarchyViewModel *model);
int umi_editor_intel_type_hierarchy_view_model_valid(const UmiEditorIntelTypeHierarchyViewModel *model);

#ifdef __cplusplus
}
#endif
#endif
