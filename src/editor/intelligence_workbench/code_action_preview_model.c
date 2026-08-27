/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/code_action_preview_model.c
 *
 * PURPOSE:
 *   Represent the preview state of a resolved code action.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/code_action_preview_model.h"

#include <string.h>

UmiStatus umi_editor_intel_code_action_preview_model_init(UmiEditorIntelCodeActionPreviewModel *model,const char *id,const char *label,const char *path,UmiEditorIntelRange range){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);if(umi_editor_intel_entry_init(&model->value,id,label,path,range)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;model->applicability=UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE;model->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_code_action_preview_model_set_score(UmiEditorIntelCodeActionPreviewModel *model,uint32_t score){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->value.score=score;model->value.revision++;model->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_code_action_preview_model_set_selected(UmiEditorIntelCodeActionPreviewModel *model,bool selected){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;model->selected=selected;model->revision++;return UMI_STATUS_OK;}
int umi_editor_intel_code_action_preview_model_valid(const UmiEditorIntelCodeActionPreviewModel *model){return model!=NULL&&umi_editor_intel_entry_valid(&model->value)&&model->applicability>=UMI_EDITOR_INTEL_APPLICABILITY_DISABLED&&model->applicability<=UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED;}
