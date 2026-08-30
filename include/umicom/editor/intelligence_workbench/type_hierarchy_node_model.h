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

typedef struct UmiEditorIntelTypeHierarchyNodeModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelTypeHierarchyNodeModel;
UmiStatus umi_editor_intel_type_hierarchy_node_model_init(UmiEditorIntelTypeHierarchyNodeModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_type_hierarchy_node_model_set_score(UmiEditorIntelTypeHierarchyNodeModel *model,uint32_t score);
UmiStatus umi_editor_intel_type_hierarchy_node_model_set_selected(UmiEditorIntelTypeHierarchyNodeModel *model,bool selected);
int umi_editor_intel_type_hierarchy_node_model_valid(const UmiEditorIntelTypeHierarchyNodeModel *model);

#ifdef __cplusplus
}
#endif
#endif
