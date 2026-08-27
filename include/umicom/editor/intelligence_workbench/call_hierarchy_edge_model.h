/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/intelligence_workbench/call_hierarchy_edge_model.h
 *
 * PURPOSE:
 *   Model call hierarchy edge model as toolkit-neutral Framework-owned editor intelligence state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CALL_HIERARCHY_EDGE_MODEL_H
#define UMICOM_EDITOR_INTELLIGENCE_WORKBENCH_CALL_HIERARCHY_EDGE_MODEL_H

#include "umicom/editor/intelligence_workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorIntelCallHierarchyEdgeModel { UmiEditorIntelEntry value; UmiEditorIntelApplicability applicability; bool selected; uint64_t revision; } UmiEditorIntelCallHierarchyEdgeModel;
UmiStatus umi_editor_intel_call_hierarchy_edge_model_init(UmiEditorIntelCallHierarchyEdgeModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range);
UmiStatus umi_editor_intel_call_hierarchy_edge_model_set_score(UmiEditorIntelCallHierarchyEdgeModel *model,uint32_t score);
UmiStatus umi_editor_intel_call_hierarchy_edge_model_set_selected(UmiEditorIntelCallHierarchyEdgeModel *model,bool selected);
int umi_editor_intel_call_hierarchy_edge_model_valid(const UmiEditorIntelCallHierarchyEdgeModel *model);

#ifdef __cplusplus
}
#endif
#endif
