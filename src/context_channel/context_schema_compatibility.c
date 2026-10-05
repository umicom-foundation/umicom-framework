/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_schema_compatibility.c
 *
 * PURPOSE:
 *   Implement evaluate schema compatibility before cross-application delivery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_schema_compatibility.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context schema compatibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_schema_compatibility_init(UmiContextSchemaCompatibility *state)
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
 * Provide the context schema compatibility set field operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_schema_compatibility_set_field(UmiContextSchemaCompatibility *state,size_t field_index,const char *value)
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context schema compatibility field operation used by this module and its
 * client applications.
 */
const char *umi_context_schema_compatibility_field(const UmiContextSchemaCompatibility *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->check_id;
    case 1U: return state->source_schema;
    case 2U: return state->target_schema;
    case 3U: return state->message;
    default:return NULL;
    
}
}
/*
 * Provide the context schema compatibility record success operation used by this module
 * and its client applications.
 */
UmiStatus umi_context_schema_compatibility_record_success(UmiContextSchemaCompatibility *state,uint64_t sequence)
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
 * Provide the context schema compatibility record failure operation used by this module
 * and its client applications.
 */
UmiStatus umi_context_schema_compatibility_record_failure(UmiContextSchemaCompatibility *state,UmiStatus status,uint64_t sequence)
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
 * Check that context schema compatibility satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_context_schema_compatibility_validate(const UmiContextSchemaCompatibility *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->check_id, '\0', sizeof(state->check_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->source_schema, '\0', sizeof(state->source_schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->target_schema, '\0', sizeof(state->target_schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->message, '\0', sizeof(state->message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->check_id,sizeof(state->check_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->source_schema,sizeof(state->source_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->target_schema,sizeof(state->target_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->message,sizeof(state->message)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context schema compatibility covers sequence operation used by this module
 * and its client applications.
 */
bool umi_context_schema_compatibility_covers_sequence(const UmiContextSchemaCompatibility *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextSchemaCompatibilityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4aae29615607574e);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchemaCompatibility *)0)->check_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchemaCompatibility *)0)->source_schema)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchemaCompatibility *)0)->target_schema)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchemaCompatibility *)0)->message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextSchemaCompatibilityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextSchemaCompatibility *)0)->check_id) - 1U +
        8U + sizeof(((UmiContextSchemaCompatibility *)0)->source_schema) - 1U +
        8U + sizeof(((UmiContextSchemaCompatibility *)0)->target_schema) - 1U +
        8U + sizeof(((UmiContextSchemaCompatibility *)0)->message) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextSchemaCompatibilityArchiveWrite(UmiArchiveWriter *writer, const UmiContextSchemaCompatibility *value)
{
    UmiArchiveWriteText(writer, value->check_id, sizeof(value->check_id));
    UmiArchiveWriteText(writer, value->source_schema, sizeof(value->source_schema));
    UmiArchiveWriteText(writer, value->target_schema, sizeof(value->target_schema));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextSchemaCompatibilityArchiveRead(UmiArchiveReader *reader, UmiContextSchemaCompatibility *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->check_id, sizeof(value->check_id));
    UmiArchiveReadText(reader, value->source_schema, sizeof(value->source_schema));
    UmiArchiveReadText(reader, value->target_schema, sizeof(value->target_schema));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextSchemaCompatibilityArchiveValidate(const UmiContextSchemaCompatibility *value)
{
    return umi_context_schema_compatibility_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_schema_compatibility_archive_encode, umi_context_schema_compatibility_archive_decode,
    UmiContextSchemaCompatibility, UmiContextSchemaCompatibilityArchiveSchema, UmiContextSchemaCompatibilityArchiveBound, UmiContextSchemaCompatibilityArchiveWrite, UmiContextSchemaCompatibilityArchiveRead, UmiContextSchemaCompatibilityArchiveValidate)
