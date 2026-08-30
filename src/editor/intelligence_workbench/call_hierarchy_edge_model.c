/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/call_hierarchy_edge_model.c
 *
 * PURPOSE:
 *   Model call hierarchy edge model as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/call_hierarchy_edge_model.h"

#include <string.h>

UmiStatus umi_editor_intel_call_hierarchy_edge_model_init(UmiEditorIntelCallHierarchyEdgeModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);if(umi_editor_intel_entry_init(&model->value,id,label,path,range)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->applicability=UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE;model->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_call_hierarchy_edge_model_set_score(UmiEditorIntelCallHierarchyEdgeModel *model,uint32_t score){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->value.score=score;model->value.revision++;model->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_call_hierarchy_edge_model_set_selected(UmiEditorIntelCallHierarchyEdgeModel *model,bool selected){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->selected=selected;model->revision++;return UMI_STATUS_OK;}
int umi_editor_intel_call_hierarchy_edge_model_valid(const UmiEditorIntelCallHierarchyEdgeModel *model){return model!=NULL&&umi_editor_intel_entry_valid(&model->value)&&model->applicability>=UMI_EDITOR_INTEL_APPLICABILITY_DISABLED&&model->applicability<=UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED;}
