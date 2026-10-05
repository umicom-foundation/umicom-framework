/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_import.c
 *
 * PURPOSE:
 *   Implement validate and stage imported context records before publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_import.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context import from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_import_init(UmiContextImport *state)
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
 * Provide the context import set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_import_set_field(UmiContextImport *state,size_t field_index,const char *value)
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
    case 0U: target = state->import_id;
capacity = sizeof(state->import_id);
break;
    case 1U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 2U: target = state->source_name;
capacity = sizeof(state->source_name);
break;
    case 3U: target = state->target_channel;
capacity = sizeof(state->target_channel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context import field operation used by this module and its client
 * applications.
 */
const char *umi_context_import_field(const UmiContextImport *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->import_id;
    case 1U: return state->schema_id;
    case 2U: return state->source_name;
    case 3U: return state->target_channel;
    default:return NULL;
    
}
}
/*
 * Provide the context import record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_import_record_success(UmiContextImport *state,uint64_t sequence)
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
 * Provide the context import record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_import_record_failure(UmiContextImport *state,UmiStatus status,uint64_t sequence)
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
/* Check that context import satisfies its contract before another service relies on it. */
UmiStatus umi_context_import_validate(const UmiContextImport *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->import_id, '\0', sizeof(state->import_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->schema_id, '\0', sizeof(state->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->source_name, '\0', sizeof(state->source_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->target_channel, '\0', sizeof(state->target_channel)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->import_id,sizeof(state->import_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->source_name,sizeof(state->source_name)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->target_channel,sizeof(state->target_channel)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context import covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_import_covers_sequence(const UmiContextImport *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextImportArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7b7c4ebde5f68100);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextImport *)0)->import_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextImport *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextImport *)0)->source_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextImport *)0)->target_channel)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextImportArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextImport *)0)->import_id) - 1U +
        8U + sizeof(((UmiContextImport *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextImport *)0)->source_name) - 1U +
        8U + sizeof(((UmiContextImport *)0)->target_channel) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextImportArchiveWrite(UmiArchiveWriter *writer, const UmiContextImport *value)
{
    UmiArchiveWriteText(writer, value->import_id, sizeof(value->import_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->source_name, sizeof(value->source_name));
    UmiArchiveWriteText(writer, value->target_channel, sizeof(value->target_channel));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextImportArchiveRead(UmiArchiveReader *reader, UmiContextImport *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->import_id, sizeof(value->import_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->source_name, sizeof(value->source_name));
    UmiArchiveReadText(reader, value->target_channel, sizeof(value->target_channel));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextImportArchiveValidate(const UmiContextImport *value)
{
    return umi_context_import_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_import_archive_encode, umi_context_import_archive_decode,
    UmiContextImport, UmiContextImportArchiveSchema, UmiContextImportArchiveBound, UmiContextImportArchiveWrite, UmiContextImportArchiveRead, UmiContextImportArchiveValidate)
