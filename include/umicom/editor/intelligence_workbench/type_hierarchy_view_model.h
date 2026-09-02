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

/**
 * Represent the editor intel type hierarchy view model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelTypeHierarchyViewModel { UmiEditorIntelEntry items[UMI_EDITOR_INTEL_MAX_ITEMS]; size_t count; uint64_t revision; } UmiEditorIntelTypeHierarchyViewModel;
/**
 * Initialise editor intel type hierarchy view model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_type_hierarchy_view_model_init(UmiEditorIntelTypeHierarchyViewModel *model);
/**
 * Add editor intel type hierarchy view model only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_editor_intel_type_hierarchy_view_model_add(UmiEditorIntelTypeHierarchyViewModel *model,const UmiEditorIntelEntry *entry);
/**
 * Find editor intel type hierarchy view model while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_type_hierarchy_view_model_find(const UmiEditorIntelTypeHierarchyViewModel *model,const char *id);
/**
 * Release or reset state held by editor intel type hierarchy view model so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_intel_type_hierarchy_view_model_clear(UmiEditorIntelTypeHierarchyViewModel *model);
/**
 * Check that editor intel type hierarchy view model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_type_hierarchy_view_model_valid(const UmiEditorIntelTypeHierarchyViewModel *model);

#ifdef __cplusplus
}
#endif
#endif
