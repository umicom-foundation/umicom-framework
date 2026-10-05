/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_export.c
 *
 * PURPOSE:
 *   Implement plan portable context exports without leaking application-private state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_export.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context export from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_export_init(UmiContextExport *state)
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
 * Provide the context export set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_export_set_field(UmiContextExport *state,size_t field_index,const char *value)
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
    case 0U: target = state->export_id;
capacity = sizeof(state->export_id);
break;
    case 1U: target = state->channel_id;
capacity = sizeof(state->channel_id);
break;
    case 2U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 3U: target = state->destination;
capacity = sizeof(state->destination);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context export field operation used by this module and its client
 * applications.
 */
const char *umi_context_export_field(const UmiContextExport *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->export_id;
    case 1U: return state->channel_id;
    case 2U: return state->schema_id;
    case 3U: return state->destination;
    default:return NULL;
    
}
}
/*
 * Provide the context export record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_export_record_success(UmiContextExport *state,uint64_t sequence)
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
 * Provide the context export record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_export_record_failure(UmiContextExport *state,UmiStatus status,uint64_t sequence)
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
/* Check that context export satisfies its contract before another service relies on it. */
UmiStatus umi_context_export_validate(const UmiContextExport *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->export_id, '\0', sizeof(state->export_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->channel_id, '\0', sizeof(state->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->schema_id, '\0', sizeof(state->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->destination, '\0', sizeof(state->destination)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->export_id,sizeof(state->export_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->destination,sizeof(state->destination)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context export covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_export_covers_sequence(const UmiContextExport *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextExportArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2baed6b31df7787b);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextExport *)0)->export_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextExport *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextExport *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextExport *)0)->destination)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextExportArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextExport *)0)->export_id) - 1U +
        8U + sizeof(((UmiContextExport *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextExport *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextExport *)0)->destination) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextExportArchiveWrite(UmiArchiveWriter *writer, const UmiContextExport *value)
{
    UmiArchiveWriteText(writer, value->export_id, sizeof(value->export_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->destination, sizeof(value->destination));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextExportArchiveRead(UmiArchiveReader *reader, UmiContextExport *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->export_id, sizeof(value->export_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->destination, sizeof(value->destination));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextExportArchiveValidate(const UmiContextExport *value)
{
    return umi_context_export_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_export_archive_encode, umi_context_export_archive_decode,
    UmiContextExport, UmiContextExportArchiveSchema, UmiContextExportArchiveBound, UmiContextExportArchiveWrite, UmiContextExportArchiveRead, UmiContextExportArchiveValidate)
