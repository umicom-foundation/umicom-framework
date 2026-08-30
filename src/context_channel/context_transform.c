/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_transform.c
 *
 * PURPOSE:
 *   Implement record deterministic schema transformation plans for context values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_transform.h"
#include <string.h>
void umi_context_transform_init(UmiContextTransform *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_transform_set_field(UmiContextTransform *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->plan_id;
capacity = sizeof(state->plan_id);
break;
    case 1U: target = state->source_schema;
capacity = sizeof(state->source_schema);
break;
    case 2U: target = state->target_schema;
capacity = sizeof(state->target_schema);
break;
    case 3U: target = state->transformer_id;
capacity = sizeof(state->transformer_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_transform_field(const UmiContextTransform *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->plan_id;
    case 1U: return state->source_schema;
    case 2U: return state->target_schema;
    case 3U: return state->transformer_id;
    default:return NULL;
    
}
}
UmiStatus umi_context_transform_record_success(UmiContextTransform *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_transform_record_failure(UmiContextTransform *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_transform_validate(const UmiContextTransform *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->plan_id,sizeof(state->plan_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->source_schema,sizeof(state->source_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->target_schema,sizeof(state->target_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->transformer_id,sizeof(state->transformer_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_transform_covers_sequence(const UmiContextTransform *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
