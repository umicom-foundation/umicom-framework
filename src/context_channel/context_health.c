/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_health.c
 *
 * PURPOSE:
 *   Implement expose aggregate context-channel health and degradation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_health.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context health from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_health_init(UmiContextHealth *state)
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
 * Provide the context health set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_health_set_field(UmiContextHealth *state,size_t field_index,const char *value)
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
    case 0U: target = state->health_id;
capacity = sizeof(state->health_id);
break;
    case 1U: target = state->component_id;
capacity = sizeof(state->component_id);
break;
    case 2U: target = state->message;
capacity = sizeof(state->message);
break;
    case 3U: target = state->last_failure;
capacity = sizeof(state->last_failure);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context health field operation used by this module and its client
 * applications.
 */
const char *umi_context_health_field(const UmiContextHealth *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->health_id;
    case 1U: return state->component_id;
    case 2U: return state->message;
    case 3U: return state->last_failure;
    default:return NULL;
    
}
}
/*
 * Provide the context health record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_health_record_success(UmiContextHealth *state,uint64_t sequence)
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
 * Provide the context health record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_health_record_failure(UmiContextHealth *state,UmiStatus status,uint64_t sequence)
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
/* Check that context health satisfies its contract before another service relies on it. */
UmiStatus umi_context_health_validate(const UmiContextHealth *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->health_id, '\0', sizeof(state->health_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->component_id, '\0', sizeof(state->component_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->message, '\0', sizeof(state->message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->last_failure, '\0', sizeof(state->last_failure)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->health_id,sizeof(state->health_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->component_id,sizeof(state->component_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->message,sizeof(state->message)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->last_failure,sizeof(state->last_failure)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context health covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_health_covers_sequence(const UmiContextHealth *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextHealthArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc2f4ab20c82d23cd);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHealth *)0)->health_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHealth *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHealth *)0)->message)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHealth *)0)->last_failure)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextHealthArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextHealth *)0)->health_id) - 1U +
        8U + sizeof(((UmiContextHealth *)0)->component_id) - 1U +
        8U + sizeof(((UmiContextHealth *)0)->message) - 1U +
        8U + sizeof(((UmiContextHealth *)0)->last_failure) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextHealthArchiveWrite(UmiArchiveWriter *writer, const UmiContextHealth *value)
{
    UmiArchiveWriteText(writer, value->health_id, sizeof(value->health_id));
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteText(writer, value->last_failure, sizeof(value->last_failure));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextHealthArchiveRead(UmiArchiveReader *reader, UmiContextHealth *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->health_id, sizeof(value->health_id));
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    UmiArchiveReadText(reader, value->last_failure, sizeof(value->last_failure));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextHealthArchiveValidate(const UmiContextHealth *value)
{
    return umi_context_health_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_health_archive_encode, umi_context_health_archive_decode,
    UmiContextHealth, UmiContextHealthArchiveSchema, UmiContextHealthArchiveBound, UmiContextHealthArchiveWrite, UmiContextHealthArchiveRead, UmiContextHealthArchiveValidate)
