/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_preference.c
 *
 * PURPOSE:
 *   Implement store user-facing context-link preferences independently of gtk widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_preference.h"
#include <string.h>
void umi_context_preference_init(UmiContextPreference *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_preference_set_field(UmiContextPreference *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->preference_id;
capacity = sizeof(state->preference_id);
break;
    case 1U: target = state->user_id;
capacity = sizeof(state->user_id);
break;
    case 2U: target = state->default_colour;
capacity = sizeof(state->default_colour);
break;
    case 3U: target = state->default_channel;
capacity = sizeof(state->default_channel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_preference_field(const UmiContextPreference *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->preference_id;
    case 1U: return state->user_id;
    case 2U: return state->default_colour;
    case 3U: return state->default_channel;
    default:return NULL;
    
}
}
UmiStatus umi_context_preference_record_success(UmiContextPreference *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_preference_record_failure(UmiContextPreference *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_preference_validate(const UmiContextPreference *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->preference_id,sizeof(state->preference_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->user_id,sizeof(state->user_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->default_colour,sizeof(state->default_colour)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->default_channel,sizeof(state->default_channel)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_preference_covers_sequence(const UmiContextPreference *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
