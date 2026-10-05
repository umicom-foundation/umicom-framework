/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_sync.c
 *
 * PURPOSE:
 *   Implement track cross-process context synchronisation cursors and bounded lag.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_sync.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context sync from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_sync_init(UmiContextSync *state)
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
 * Provide the context sync set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_sync_set_field(UmiContextSync *state,size_t field_index,const char *value)
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
    case 0U: target = state->peer_id;
capacity = sizeof(state->peer_id);
break;
    case 1U: target = state->channel_id;
capacity = sizeof(state->channel_id);
break;
    case 2U: target = state->last_context_id;
capacity = sizeof(state->last_context_id);
break;
    case 3U: target = state->error_text;
capacity = sizeof(state->error_text);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context sync field operation used by this module and its client
 * applications.
 */
const char *umi_context_sync_field(const UmiContextSync *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->peer_id;
    case 1U: return state->channel_id;
    case 2U: return state->last_context_id;
    case 3U: return state->error_text;
    default:return NULL;
    
}
}
/*
 * Provide the context sync record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_sync_record_success(UmiContextSync *state,uint64_t sequence)
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
 * Provide the context sync record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_sync_record_failure(UmiContextSync *state,UmiStatus status,uint64_t sequence)
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
/* Check that context sync satisfies its contract before another service relies on it. */
UmiStatus umi_context_sync_validate(const UmiContextSync *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->peer_id, '\0', sizeof(state->peer_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->channel_id, '\0', sizeof(state->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->last_context_id, '\0', sizeof(state->last_context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->error_text, '\0', sizeof(state->error_text)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->peer_id,sizeof(state->peer_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->last_context_id,sizeof(state->last_context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->error_text,sizeof(state->error_text)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context sync covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_sync_covers_sequence(const UmiContextSync *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextSyncArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0a5dcb0074877a9a);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSync *)0)->peer_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSync *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSync *)0)->last_context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSync *)0)->error_text)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextSyncArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextSync *)0)->peer_id) - 1U +
        8U + sizeof(((UmiContextSync *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextSync *)0)->last_context_id) - 1U +
        8U + sizeof(((UmiContextSync *)0)->error_text) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextSyncArchiveWrite(UmiArchiveWriter *writer, const UmiContextSync *value)
{
    UmiArchiveWriteText(writer, value->peer_id, sizeof(value->peer_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->last_context_id, sizeof(value->last_context_id));
    UmiArchiveWriteText(writer, value->error_text, sizeof(value->error_text));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextSyncArchiveRead(UmiArchiveReader *reader, UmiContextSync *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->peer_id, sizeof(value->peer_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->last_context_id, sizeof(value->last_context_id));
    UmiArchiveReadText(reader, value->error_text, sizeof(value->error_text));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextSyncArchiveValidate(const UmiContextSync *value)
{
    return umi_context_sync_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_sync_archive_encode, umi_context_sync_archive_decode,
    UmiContextSync, UmiContextSyncArchiveSchema, UmiContextSyncArchiveBound, UmiContextSyncArchiveWrite, UmiContextSyncArchiveRead, UmiContextSyncArchiveValidate)
