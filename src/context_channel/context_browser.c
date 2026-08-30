/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_browser.c
 *
 * PURPOSE:
 *   Implement build immutable browser rows for recent, pinned and active contexts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_browser.h"
#include <string.h>
void umi_context_browser_init(UmiContextBrowser *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_browser_set_field(UmiContextBrowser *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->filter_text;
capacity = sizeof(state->filter_text);
break;
    case 1U: target = state->category;
capacity = sizeof(state->category);
break;
    case 2U: target = state->owner_id;
capacity = sizeof(state->owner_id);
break;
    case 3U: target = state->workspace_id;
capacity = sizeof(state->workspace_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_browser_field(const UmiContextBrowser *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->filter_text;
    case 1U: return state->category;
    case 2U: return state->owner_id;
    case 3U: return state->workspace_id;
    default:return NULL;
    
}
}
UmiStatus umi_context_browser_record_success(UmiContextBrowser *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_browser_record_failure(UmiContextBrowser *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_browser_validate(const UmiContextBrowser *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->filter_text,sizeof(state->filter_text)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->category,sizeof(state->category)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->owner_id,sizeof(state->owner_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->workspace_id,sizeof(state->workspace_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_browser_covers_sequence(const UmiContextBrowser *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
