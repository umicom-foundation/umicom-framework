/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_replay.c
 *
 * PURPOSE:
 *   Implement replay retained contexts with a deterministic cursor and delivery budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_replay.h"
#include <string.h>
void umi_context_replay_init(UmiContextReplay *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_replay_set_field(UmiContextReplay *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->channel_id;
capacity = sizeof(state->channel_id);
break;
    case 1U: target = state->from_context_id;
capacity = sizeof(state->from_context_id);
break;
    case 2U: target = state->target_application;
capacity = sizeof(state->target_application);
break;
    case 3U: target = state->target_panel;
capacity = sizeof(state->target_panel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_replay_field(const UmiContextReplay *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->channel_id;
    case 1U: return state->from_context_id;
    case 2U: return state->target_application;
    case 3U: return state->target_panel;
    default:return NULL;
    
}
}
UmiStatus umi_context_replay_record_success(UmiContextReplay *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_replay_record_failure(UmiContextReplay *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_replay_validate(const UmiContextReplay *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->from_context_id,sizeof(state->from_context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->target_application,sizeof(state->target_application)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->target_panel,sizeof(state->target_panel)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_replay_covers_sequence(const UmiContextReplay *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
