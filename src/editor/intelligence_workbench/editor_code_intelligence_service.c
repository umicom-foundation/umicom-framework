/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/editor_code_intelligence_service.c
 *
 * PURPOSE:
 *   Aggregate Framework-owned search, navigation, code-action and refactoring state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical editor/language
 *   services; Studio remains a thin frontend and owns no reusable semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/editor_code_intelligence_service.h"

#include <string.h>

UmiStatus umi_editor_intel_editor_code_intelligence_service_init(UmiEditorIntelEditorCodeIntelligenceService *model){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);model->revision=1U;return UMI_STATUS_OK;}
const UmiEditorIntelEntry *umi_editor_intel_editor_code_intelligence_service_find(const UmiEditorIntelEditorCodeIntelligenceService *model,const char *id){size_t index;if(model==NULL||!umi_editor_intel_id_valid(id))return NULL;for(index=0U;index<model->count;index++){if(strcmp(model->items[index].id,id)==0)return &model->items[index];}return NULL;}
UmiStatus umi_editor_intel_editor_code_intelligence_service_add(UmiEditorIntelEditorCodeIntelligenceService *model,const UmiEditorIntelEntry *entry){if(model==NULL||!umi_editor_intel_entry_valid(entry))return UMI_STATUS_INVALID_ARGUMENT;if(umi_editor_intel_editor_code_intelligence_service_find(model,entry->id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(model->count>=UMI_EDITOR_INTEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;model->items[model->count]=*entry;model->count++;model->revision++;return UMI_STATUS_OK;}
UmiStatus umi_editor_intel_editor_code_intelligence_service_clear(UmiEditorIntelEditorCodeIntelligenceService *model){if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model->items,0,sizeof model->items);model->count=0U;model->revision++;return UMI_STATUS_OK;}
int umi_editor_intel_editor_code_intelligence_service_valid(const UmiEditorIntelEditorCodeIntelligenceService *model){size_t index;if(model==NULL||model->count>UMI_EDITOR_INTEL_MAX_ITEMS)return 0;for(index=0U;index<model->count;index++){if(!umi_editor_intel_entry_valid(&model->items[index]))return 0;}return 1;}
