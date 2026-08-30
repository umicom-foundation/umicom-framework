/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_snapshot_store.c
 *
 * PURPOSE:
 *   Implement retain named immutable context snapshots for debugging and comparison.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_snapshot_store.h"
#include <string.h>
void umi_context_snapshot_store_init(UmiContextSnapshotStore *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_snapshot_store_set_field(UmiContextSnapshotStore *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->snapshot_id;
capacity = sizeof(state->snapshot_id);
break;
    case 1U: target = state->context_id;
capacity = sizeof(state->context_id);
break;
    case 2U: target = state->channel_id;
capacity = sizeof(state->channel_id);
break;
    case 3U: target = state->label;
capacity = sizeof(state->label);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_snapshot_store_field(const UmiContextSnapshotStore *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->snapshot_id;
    case 1U: return state->context_id;
    case 2U: return state->channel_id;
    case 3U: return state->label;
    default:return NULL;
    
}
}
UmiStatus umi_context_snapshot_store_record_success(UmiContextSnapshotStore *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_snapshot_store_record_failure(UmiContextSnapshotStore *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_snapshot_store_validate(const UmiContextSnapshotStore *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->snapshot_id,sizeof(state->snapshot_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->context_id,sizeof(state->context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->label,sizeof(state->label)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_snapshot_store_covers_sequence(const UmiContextSnapshotStore *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
