/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_backpressure.c
 *
 * PURPOSE:
 *   Implement expose subscriber backlog and delivery pressure for operational control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_backpressure.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context backpressure from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_backpressure_init(UmiContextBackpressure *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return;
    memset(state,0,sizeof(*state));
    state->structure_size=(uint32_t)sizeof(*state);
    state->enabled=true;
    state->status=UMI_STATUS_OK;
    state->revision=1U;
}
/*
 * Provide the context backpressure set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_backpressure_set_field(UmiContextBackpressure *state,size_t field_index,const char *value)
{
    char *target=NULL;
size_t capacity=0U;
UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||value==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: target = state->pressure_id;
capacity = sizeof(state->pressure_id);
break;
    case 1U: target = state->channel_id;
capacity = sizeof(state->channel_id);
break;
    case 2U: target = state->subscription_id;
capacity = sizeof(state->subscription_id);
break;
    case 3U: target = state->message;
capacity = sizeof(state->message);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context backpressure field operation used by this module and its client
 * applications.
 */
const char *umi_context_backpressure_field(const UmiContextBackpressure *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->pressure_id;
    case 1U: return state->channel_id;
    case 2U: return state->subscription_id;
    case 3U: return state->message;
    default:return NULL;
    
}
}
/*
 * Provide the context backpressure record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_backpressure_record_success(UmiContextBackpressure *state,uint64_t sequence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||!state->enabled)return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->status=UMI_STATUS_OK;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the context backpressure record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_backpressure_record_failure(UmiContextBackpressure *state,UmiStatus status,uint64_t sequence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if(state->item_count==0U)state->first_sequence=sequence;
    state->last_sequence=sequence;
    state->item_count+=1U;
    state->failure_count+=1U;
    state->status=status;
    state->revision+=1U;
    return UMI_STATUS_OK;
}
/*
 * Check that context backpressure satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_backpressure_validate(const UmiContextBackpressure *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->pressure_id, '\0', sizeof(state->pressure_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->channel_id, '\0', sizeof(state->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->subscription_id, '\0', sizeof(state->subscription_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->message, '\0', sizeof(state->message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->pressure_id,sizeof(state->pressure_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->subscription_id,sizeof(state->subscription_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->message,sizeof(state->message)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context backpressure covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_backpressure_covers_sequence(const UmiContextBackpressure *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextBackpressureArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdbec4f2341f17a5b);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextBackpressure *)0)->pressure_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextBackpressure *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextBackpressure *)0)->subscription_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextBackpressure *)0)->message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextBackpressureArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextBackpressure *)0)->pressure_id) - 1U +
        8U + sizeof(((UmiContextBackpressure *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextBackpressure *)0)->subscription_id) - 1U +
        8U + sizeof(((UmiContextBackpressure *)0)->message) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextBackpressureArchiveWrite(UmiArchiveWriter *writer, const UmiContextBackpressure *value)
{
    UmiArchiveWriteText(writer, value->pressure_id, sizeof(value->pressure_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->subscription_id, sizeof(value->subscription_id));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextBackpressureArchiveRead(UmiArchiveReader *reader, UmiContextBackpressure *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->pressure_id, sizeof(value->pressure_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->subscription_id, sizeof(value->subscription_id));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextBackpressureArchiveValidate(const UmiContextBackpressure *value)
{
    return umi_context_backpressure_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_backpressure_archive_encode, umi_context_backpressure_archive_decode,
    UmiContextBackpressure, UmiContextBackpressureArchiveSchema, UmiContextBackpressureArchiveBound, UmiContextBackpressureArchiveWrite, UmiContextBackpressureArchiveRead, UmiContextBackpressureArchiveValidate)
