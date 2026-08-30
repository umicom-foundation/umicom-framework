/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_inspector.c
 *
 * PURPOSE:
 *   Implement expose a read-only property projection for one typed context payload.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_inspector.h"
#include <string.h>
void umi_context_inspector_init(UmiContextInspector *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_inspector_set_field(UmiContextInspector *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->context_id;
capacity = sizeof(state->context_id);
break;
    case 1U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 2U: target = state->source_application;
capacity = sizeof(state->source_application);
break;
    case 3U: target = state->source_panel;
capacity = sizeof(state->source_panel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_inspector_field(const UmiContextInspector *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->context_id;
    case 1U: return state->schema_id;
    case 2U: return state->source_application;
    case 3U: return state->source_panel;
    default:return NULL;
    
}
}
UmiStatus umi_context_inspector_record_success(UmiContextInspector *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_inspector_record_failure(UmiContextInspector *state,UmiStatus status,uint64_t sequence)
{
    if(state==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->failure_count+=1U;
    state->status=status;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_inspector_validate(const UmiContextInspector *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->context_id,sizeof(state->context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->source_application,sizeof(state->source_application)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->source_panel,sizeof(state->source_panel)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_inspector_covers_sequence(const UmiContextInspector *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
