/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/code_intelligence_command_set.c
 *
 * PURPOSE:
 *   Model code intelligence command set as toolkit-neutral Framework-owned editor intelligence state.
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
#include "umicom/editor/intelligence_workbench/code_intelligence_command_set.h"

#include <string.h>

/*
 * Initialise editor intel code intelligence command set from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_editor_intel_code_intelligence_command_set_init(UmiEditorIntelCodeIntelligenceCommandSet *model){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model,0,sizeof *model);model->revision=1U;return UMI_STATUS_OK;}
/*
 * Find editor intel code intelligence command set while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_code_intelligence_command_set_find(const UmiEditorIntelCodeIntelligenceCommandSet *model,const char *id){size_t index;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||!umi_editor_intel_id_valid(id))return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(index=0U;index<model->count;index++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(model->items[index].id,id)==0)return &model->items[index];}return NULL;}
/*
 * Add editor intel code intelligence command set only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_editor_intel_code_intelligence_command_set_add(UmiEditorIntelCodeIntelligenceCommandSet *model,const UmiEditorIntelEntry *entry){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||!umi_editor_intel_entry_valid(entry))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_intel_code_intelligence_command_set_find(model,entry->id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model->count>=UMI_EDITOR_INTEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;model->items[model->count]=*entry;model->count++;model->revision++;return UMI_STATUS_OK;}
/*
 * Release or reset state held by editor intel code intelligence command set so the same
 * storage can be reused safely.
 */
UmiStatus umi_editor_intel_code_intelligence_command_set_clear(UmiEditorIntelCodeIntelligenceCommandSet *model){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(model->items,0,sizeof model->items);model->count=0U;model->revision++;return UMI_STATUS_OK;}
/*
 * Check that editor intel code intelligence command set satisfies its contract before
 * another service relies on it.
 */
int umi_editor_intel_code_intelligence_command_set_valid(const UmiEditorIntelCodeIntelligenceCommandSet *model){size_t index;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(model==NULL||model->count>UMI_EDITOR_INTEL_MAX_ITEMS)return 0;/* Visit each bounded item once so every record receives the same rule. */ for(index=0U;index<model->count;index++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_editor_intel_entry_valid(&model->items[index]))return 0;}return 1;}
