/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_codec.c
 *
 * PURPOSE:
 *   Implement describe codec selection for local ipc, json and future binary transports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_codec.h"
#include <string.h>
void umi_context_codec_init(UmiContextCodec *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_codec_set_field(UmiContextCodec *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->codec_id;
capacity = sizeof(state->codec_id);
break;
    case 1U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 2U: target = state->media_type;
capacity = sizeof(state->media_type);
break;
    case 3U: target = state->encoding;
capacity = sizeof(state->encoding);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_codec_field(const UmiContextCodec *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->codec_id;
    case 1U: return state->schema_id;
    case 2U: return state->media_type;
    case 3U: return state->encoding;
    default:return NULL;
    
}
}
UmiStatus umi_context_codec_record_success(UmiContextCodec *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_codec_record_failure(UmiContextCodec *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_codec_validate(const UmiContextCodec *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->codec_id,sizeof(state->codec_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->media_type,sizeof(state->media_type)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->encoding,sizeof(state->encoding)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_codec_covers_sequence(const UmiContextCodec *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
