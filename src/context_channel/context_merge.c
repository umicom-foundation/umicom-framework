/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_merge.c
 *
 * PURPOSE:
 *   Implement record explicit context merge choices and resulting evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_merge.h"
#include <string.h>
void umi_context_merge_init(UmiContextMerge *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_merge_set_field(UmiContextMerge *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->merge_id;
capacity = sizeof(state->merge_id);
break;
    case 1U: target = state->conflict_id;
capacity = sizeof(state->conflict_id);
break;
    case 2U: target = state->resolution;
capacity = sizeof(state->resolution);
break;
    case 3U: target = state->result_context_id;
capacity = sizeof(state->result_context_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_merge_field(const UmiContextMerge *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->merge_id;
    case 1U: return state->conflict_id;
    case 2U: return state->resolution;
    case 3U: return state->result_context_id;
    default:return NULL;
    
}
}
UmiStatus umi_context_merge_record_success(UmiContextMerge *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_merge_record_failure(UmiContextMerge *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_merge_validate(const UmiContextMerge *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->merge_id,sizeof(state->merge_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->conflict_id,sizeof(state->conflict_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->resolution,sizeof(state->resolution)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->result_context_id,sizeof(state->result_context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_merge_covers_sequence(const UmiContextMerge *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
