/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_merge.c
 *
 * PURPOSE:
 *   Implement record explicit context merge choices and resulting evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_merge.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context merge from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_merge_init(UmiContextMerge *state)
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
 * Provide the context merge set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_merge_set_field(UmiContextMerge *state,size_t field_index,const char *value)
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
    case 0U: target = state->merge_id;
capacity = sizeof(state->merge_id);
break;
    case 1U: target = state->conflict_id;
capacity = sizeof(state->conflict_id);
break;
    case 2U: target = state->resolution;
capacity = sizeof(state->resolution);
break;
    case 3U: target = state->result_context_id;
capacity = sizeof(state->result_context_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context merge field operation used by this module and its client
 * applications.
 */
const char *umi_context_merge_field(const UmiContextMerge *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->merge_id;
    case 1U: return state->conflict_id;
    case 2U: return state->resolution;
    case 3U: return state->result_context_id;
    default:return NULL;
    
}
}
/*
 * Provide the context merge record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_merge_record_success(UmiContextMerge *state,uint64_t sequence)
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
 * Provide the context merge record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_merge_record_failure(UmiContextMerge *state,UmiStatus status,uint64_t sequence)
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
/* Check that context merge satisfies its contract before another service relies on it. */
UmiStatus umi_context_merge_validate(const UmiContextMerge *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->merge_id, '\0', sizeof(state->merge_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->conflict_id, '\0', sizeof(state->conflict_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->resolution, '\0', sizeof(state->resolution)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->result_context_id, '\0', sizeof(state->result_context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->merge_id,sizeof(state->merge_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->conflict_id,sizeof(state->conflict_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->resolution,sizeof(state->resolution)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->result_context_id,sizeof(state->result_context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context merge covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_merge_covers_sequence(const UmiContextMerge *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextMergeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x94df3625575165fb);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMerge *)0)->merge_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMerge *)0)->conflict_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMerge *)0)->resolution)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMerge *)0)->result_context_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextMergeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextMerge *)0)->merge_id) - 1U +
        8U + sizeof(((UmiContextMerge *)0)->conflict_id) - 1U +
        8U + sizeof(((UmiContextMerge *)0)->resolution) - 1U +
        8U + sizeof(((UmiContextMerge *)0)->result_context_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextMergeArchiveWrite(UmiArchiveWriter *writer, const UmiContextMerge *value)
{
    UmiArchiveWriteText(writer, value->merge_id, sizeof(value->merge_id));
    UmiArchiveWriteText(writer, value->conflict_id, sizeof(value->conflict_id));
    UmiArchiveWriteText(writer, value->resolution, sizeof(value->resolution));
    UmiArchiveWriteText(writer, value->result_context_id, sizeof(value->result_context_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextMergeArchiveRead(UmiArchiveReader *reader, UmiContextMerge *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->merge_id, sizeof(value->merge_id));
    UmiArchiveReadText(reader, value->conflict_id, sizeof(value->conflict_id));
    UmiArchiveReadText(reader, value->resolution, sizeof(value->resolution));
    UmiArchiveReadText(reader, value->result_context_id, sizeof(value->result_context_id));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextMergeArchiveValidate(const UmiContextMerge *value)
{
    return umi_context_merge_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_merge_archive_encode, umi_context_merge_archive_decode,
    UmiContextMerge, UmiContextMergeArchiveSchema, UmiContextMergeArchiveBound, UmiContextMergeArchiveWrite, UmiContextMergeArchiveRead, UmiContextMergeArchiveValidate)
