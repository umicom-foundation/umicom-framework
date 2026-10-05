/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_transform.c
 *
 * PURPOSE:
 *   Implement record deterministic schema transformation plans for context values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_transform.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context transform from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_transform_init(UmiContextTransform *state)
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
 * Provide the context transform set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_transform_set_field(UmiContextTransform *state,size_t field_index,const char *value)
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
    case 0U: target = state->plan_id;
capacity = sizeof(state->plan_id);
break;
    case 1U: target = state->source_schema;
capacity = sizeof(state->source_schema);
break;
    case 2U: target = state->target_schema;
capacity = sizeof(state->target_schema);
break;
    case 3U: target = state->transformer_id;
capacity = sizeof(state->transformer_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context transform field operation used by this module and its client
 * applications.
 */
const char *umi_context_transform_field(const UmiContextTransform *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->plan_id;
    case 1U: return state->source_schema;
    case 2U: return state->target_schema;
    case 3U: return state->transformer_id;
    default:return NULL;
    
}
}
/*
 * Provide the context transform record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transform_record_success(UmiContextTransform *state,uint64_t sequence)
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
 * Provide the context transform record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transform_record_failure(UmiContextTransform *state,UmiStatus status,uint64_t sequence)
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
/* Check that context transform satisfies its contract before another service relies on it. */
UmiStatus umi_context_transform_validate(const UmiContextTransform *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->plan_id, '\0', sizeof(state->plan_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->source_schema, '\0', sizeof(state->source_schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->target_schema, '\0', sizeof(state->target_schema)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->transformer_id, '\0', sizeof(state->transformer_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->plan_id,sizeof(state->plan_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->source_schema,sizeof(state->source_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->target_schema,sizeof(state->target_schema)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->transformer_id,sizeof(state->transformer_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context transform covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_transform_covers_sequence(const UmiContextTransform *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextTransformArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfdc82835cdae5c15);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTransform *)0)->plan_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTransform *)0)->source_schema)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTransform *)0)->target_schema)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextTransform *)0)->transformer_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextTransformArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextTransform *)0)->plan_id) - 1U +
        8U + sizeof(((UmiContextTransform *)0)->source_schema) - 1U +
        8U + sizeof(((UmiContextTransform *)0)->target_schema) - 1U +
        8U + sizeof(((UmiContextTransform *)0)->transformer_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextTransformArchiveWrite(UmiArchiveWriter *writer, const UmiContextTransform *value)
{
    UmiArchiveWriteText(writer, value->plan_id, sizeof(value->plan_id));
    UmiArchiveWriteText(writer, value->source_schema, sizeof(value->source_schema));
    UmiArchiveWriteText(writer, value->target_schema, sizeof(value->target_schema));
    UmiArchiveWriteText(writer, value->transformer_id, sizeof(value->transformer_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextTransformArchiveRead(UmiArchiveReader *reader, UmiContextTransform *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->plan_id, sizeof(value->plan_id));
    UmiArchiveReadText(reader, value->source_schema, sizeof(value->source_schema));
    UmiArchiveReadText(reader, value->target_schema, sizeof(value->target_schema));
    UmiArchiveReadText(reader, value->transformer_id, sizeof(value->transformer_id));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextTransformArchiveValidate(const UmiContextTransform *value)
{
    return umi_context_transform_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_transform_archive_encode, umi_context_transform_archive_decode,
    UmiContextTransform, UmiContextTransformArchiveSchema, UmiContextTransformArchiveBound, UmiContextTransformArchiveWrite, UmiContextTransformArchiveRead, UmiContextTransformArchiveValidate)
