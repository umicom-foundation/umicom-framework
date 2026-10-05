/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_search.c
 *
 * PURPOSE:
 *   Implement search context history, panels and channels using bounded query terms.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_search.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context search from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_search_init(UmiContextSearch *state)
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
 * Provide the context search set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_search_set_field(UmiContextSearch *state,size_t field_index,const char *value)
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
    case 0U: target = state->query;
capacity = sizeof(state->query);
break;
    case 1U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 2U: target = state->application_id;
capacity = sizeof(state->application_id);
break;
    case 3U: target = state->panel_id;
capacity = sizeof(state->panel_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context search field operation used by this module and its client
 * applications.
 */
const char *umi_context_search_field(const UmiContextSearch *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->query;
    case 1U: return state->schema_id;
    case 2U: return state->application_id;
    case 3U: return state->panel_id;
    default:return NULL;
    
}
}
/*
 * Provide the context search record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_search_record_success(UmiContextSearch *state,uint64_t sequence)
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
 * Provide the context search record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_search_record_failure(UmiContextSearch *state,UmiStatus status,uint64_t sequence)
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
/* Check that context search satisfies its contract before another service relies on it. */
UmiStatus umi_context_search_validate(const UmiContextSearch *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->query, '\0', sizeof(state->query)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->schema_id, '\0', sizeof(state->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->application_id, '\0', sizeof(state->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->panel_id, '\0', sizeof(state->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->query,sizeof(state->query)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->application_id,sizeof(state->application_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->panel_id,sizeof(state->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context search covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_search_covers_sequence(const UmiContextSearch *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextSearchArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7d44b6cce83e6367);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSearch *)0)->query)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSearch *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSearch *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSearch *)0)->panel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextSearchArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextSearch *)0)->query) - 1U +
        8U + sizeof(((UmiContextSearch *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextSearch *)0)->application_id) - 1U +
        8U + sizeof(((UmiContextSearch *)0)->panel_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextSearchArchiveWrite(UmiArchiveWriter *writer, const UmiContextSearch *value)
{
    UmiArchiveWriteText(writer, value->query, sizeof(value->query));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextSearchArchiveRead(UmiArchiveReader *reader, UmiContextSearch *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->query, sizeof(value->query));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextSearchArchiveValidate(const UmiContextSearch *value)
{
    return umi_context_search_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_search_archive_encode, umi_context_search_archive_decode,
    UmiContextSearch, UmiContextSearchArchiveSchema, UmiContextSearchArchiveBound, UmiContextSearchArchiveWrite, UmiContextSearchArchiveRead, UmiContextSearchArchiveValidate)
