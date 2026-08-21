/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_schema_compatibility.c
 *
 * PURPOSE:
 *   Implement evaluate schema compatibility before cross-application delivery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_schema_compatibility.h"
#include <string.h>
void umi_context_schema_compatibility_init(UmiContextSchemaCompatibility *state)
{
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
UmiStatus umi_context_schema_compatibility_set_field(UmiContextSchemaCompatibility *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    switch(field_index){
    case 0U: target = state->check_id;
capacity = sizeof(state->check_id);
break;
    case 1U: target = state->source_schema;
capacity = sizeof(state->source_schema);
break;
    case 2U: target = state->target_schema;
capacity = sizeof(state->target_schema);
break;
    case 3U: target = state->message;
capacity = sizeof(state->message);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
const char *umi_context_schema_compatibility_field(const UmiContextSchemaCompatibility *state,size_t field_index)
{
    if(state==NULL)return NULL;
    switch(field_index){
    case 0U: return state->check_id;
    case 1U: return state->source_schema;
    case 2U: return state->target_schema;
    case 3U: return state->message;
    default:return NULL;
    
}
}
UmiStatus umi_context_schema_compatibility_record_success(UmiContextSchemaCompatibility *state,uint64_t sequence)
{
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_context_schema_compatibility_record_failure(UmiContextSchemaCompatibility *state,UmiStatus status,uint64_t sequence)
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
UmiStatus umi_context_schema_compatibility_validate(const UmiContextSchemaCompatibility *state)
{
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->check_id,sizeof(state->check_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->source_schema,sizeof(state->source_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->target_schema,sizeof(state->target_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(state->message,sizeof(state->message)))return UMI_STATUS_INVALID_ARGUMENT;
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
bool umi_context_schema_compatibility_covers_sequence(const UmiContextSchemaCompatibility *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}
