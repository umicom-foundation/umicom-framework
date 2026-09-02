/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/type_hierarchy_node_model.h
 *
 * PURPOSE:
 *   Model type hierarchy node model as toolkit-neutral Framework-owned editor intelligence state.
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
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_TYPE_HIERARCHY_NODE_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_TYPE_HIERARCHY_NODE_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor intel type hierarchy node model data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorIntelTypeHierarchyNodeModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelTypeHierarchyNodeModel;
/**
 * Initialise editor intel type hierarchy node model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_type_hierarchy_node_model_init(UmiEditorIntelTypeHierarchyNodeModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
/**
 * Provide the editor intel type hierarchy node model set score operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_type_hierarchy_node_model_set_score(UmiEditorIntelTypeHierarchyNodeModel *model,uint32_t score);
/**
 * Find editor intel type hierarchy node model set while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_editor_intel_type_hierarchy_node_model_set_selected(UmiEditorIntelTypeHierarchyNodeModel *model,bool selected);
/**
 * Check that editor intel type hierarchy node model satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_type_hierarchy_node_model_valid(const UmiEditorIntelTypeHierarchyNodeModel *model);

#ifdef __cplusplus
}
#endif
#endif
