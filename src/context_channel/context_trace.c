/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_trace.c
 *
 * PURPOSE:
 *   Implement trace context routing across applications and panels with causation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_trace.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context trace from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_trace_init(UmiContextTrace *state)
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
 * Provide the context trace set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_trace_set_field(UmiContextTrace *state,size_t field_index,const char *value)
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
    case 0U: target = state->trace_id;
capacity = sizeof(state->trace_id);
break;
    case 1U: target = state->context_id;
capacity = sizeof(state->context_id);
break;
    case 2U: target = state->correlation_id;
capacity = sizeof(state->correlation_id);
break;
    case 3U: target = state->route_id;
capacity = sizeof(state->route_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context trace field operation used by this module and its client
 * applications.
 */
const char *umi_context_trace_field(const UmiContextTrace *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->trace_id;
    case 1U: return state->context_id;
    case 2U: return state->correlation_id;
    case 3U: return state->route_id;
    default:return NULL;
    
}
}
/*
 * Provide the context trace record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_trace_record_success(UmiContextTrace *state,uint64_t sequence)
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
 * Provide the context trace record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_trace_record_failure(UmiContextTrace *state,UmiStatus status,uint64_t sequence)
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
/* Check that context trace satisfies its contract before another service relies on it. */
UmiStatus umi_context_trace_validate(const UmiContextTrace *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->trace_id, '\0', sizeof(state->trace_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->context_id, '\0', sizeof(state->context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->correlation_id, '\0', sizeof(state->correlation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->route_id, '\0', sizeof(state->route_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->trace_id,sizeof(state->trace_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->context_id,sizeof(state->context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->correlation_id,sizeof(state->correlation_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->route_id,sizeof(state->route_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context trace covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_trace_covers_sequence(const UmiContextTrace *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextTraceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5b831aa309462b3a);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTrace *)0)->trace_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTrace *)0)->context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTrace *)0)->correlation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTrace *)0)->route_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextTraceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextTrace *)0)->trace_id) - 1U +
        8U + sizeof(((UmiContextTrace *)0)->context_id) - 1U +
        8U + sizeof(((UmiContextTrace *)0)->correlation_id) - 1U +
        8U + sizeof(((UmiContextTrace *)0)->route_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextTraceArchiveWrite(UmiArchiveWriter *writer, const UmiContextTrace *value)
{
    UmiArchiveWriteText(writer, value->trace_id, sizeof(value->trace_id));
    UmiArchiveWriteText(writer, value->context_id, sizeof(value->context_id));
    UmiArchiveWriteText(writer, value->correlation_id, sizeof(value->correlation_id));
    UmiArchiveWriteText(writer, value->route_id, sizeof(value->route_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextTraceArchiveRead(UmiArchiveReader *reader, UmiContextTrace *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->trace_id, sizeof(value->trace_id));
    UmiArchiveReadText(reader, value->context_id, sizeof(value->context_id));
    UmiArchiveReadText(reader, value->correlation_id, sizeof(value->correlation_id));
    UmiArchiveReadText(reader, value->route_id, sizeof(value->route_id));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextTraceArchiveValidate(const UmiContextTrace *value)
{
    return umi_context_trace_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_trace_archive_encode, umi_context_trace_archive_decode,
    UmiContextTrace, UmiContextTraceArchiveSchema, UmiContextTraceArchiveBound, UmiContextTraceArchiveWrite, UmiContextTraceArchiveRead, UmiContextTraceArchiveValidate)
