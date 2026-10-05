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
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context codec from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_codec_init(UmiContextCodec *state)
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
 * Provide the context codec set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_codec_set_field(UmiContextCodec *state,size_t field_index,const char *value)
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context codec field operation used by this module and its client
 * applications.
 */
const char *umi_context_codec_field(const UmiContextCodec *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->codec_id;
    case 1U: return state->schema_id;
    case 2U: return state->media_type;
    case 3U: return state->encoding;
    default:return NULL;
    
}
}
/*
 * Provide the context codec record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_codec_record_success(UmiContextCodec *state,uint64_t sequence)
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
 * Provide the context codec record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_codec_record_failure(UmiContextCodec *state,UmiStatus status,uint64_t sequence)
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
/* Check that context codec satisfies its contract before another service relies on it. */
UmiStatus umi_context_codec_validate(const UmiContextCodec *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->codec_id, '\0', sizeof(state->codec_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->schema_id, '\0', sizeof(state->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->media_type, '\0', sizeof(state->media_type)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->encoding, '\0', sizeof(state->encoding)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->codec_id,sizeof(state->codec_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->media_type,sizeof(state->media_type)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->encoding,sizeof(state->encoding)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context codec covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_codec_covers_sequence(const UmiContextCodec *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextCodecArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xae7497adf95228e6);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextCodec *)0)->codec_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextCodec *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextCodec *)0)->media_type)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextCodec *)0)->encoding)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextCodecArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextCodec *)0)->codec_id) - 1U +
        8U + sizeof(((UmiContextCodec *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextCodec *)0)->media_type) - 1U +
        8U + sizeof(((UmiContextCodec *)0)->encoding) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextCodecArchiveWrite(UmiArchiveWriter *writer, const UmiContextCodec *value)
{
    UmiArchiveWriteText(writer, value->codec_id, sizeof(value->codec_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->media_type, sizeof(value->media_type));
    UmiArchiveWriteText(writer, value->encoding, sizeof(value->encoding));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextCodecArchiveRead(UmiArchiveReader *reader, UmiContextCodec *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->codec_id, sizeof(value->codec_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->media_type, sizeof(value->media_type));
    UmiArchiveReadText(reader, value->encoding, sizeof(value->encoding));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextCodecArchiveValidate(const UmiContextCodec *value)
{
    return umi_context_codec_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_codec_archive_encode, umi_context_codec_archive_decode,
    UmiContextCodec, UmiContextCodecArchiveSchema, UmiContextCodecArchiveBound, UmiContextCodecArchiveWrite, UmiContextCodecArchiveRead, UmiContextCodecArchiveValidate)
