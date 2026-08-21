/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_transport.c
 *
 * PURPOSE:
 *   Implement describe transport-neutral context delivery endpoints and capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_transport.h"
#include <string.h>
void umi_context_transport_init(UmiContextTransport *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_transport_set_field(UmiContextTransport *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->transport_id;
capacity = sizeof(state->transport_id);
break;
    case 1U: target = state->endpoint_id;
capacity = sizeof(state->endpoint_id);
break;
    case 2U: target = state->protocol;
capacity = sizeof(state->protocol);
break;
    case 3U: target = state->address;
capacity = sizeof(state->address);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_transport_field(const UmiContextTransport *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->transport_id;
    case 1U: return state->endpoint_id;
    case 2U: return state->protocol;
    case 3U: return state->address;
    default:return NULL;
    
}
}
UmiStatus umi_context_transport_record_success(UmiContextTransport *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_transport_record_failure(UmiContextTransport *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_transport_validate(const UmiContextTransport *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->transport_id,sizeof(state->transport_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->endpoint_id,sizeof(state->endpoint_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->protocol,sizeof(state->protocol)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->address,sizeof(state->address)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_transport_covers_sequence(const UmiContextTransport *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
