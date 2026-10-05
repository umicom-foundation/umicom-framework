/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_subscription_service.c
 *
 * PURPOSE:
 *   Implement coordinate subscription lifecycle and subscriber checkpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_subscription_service.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context subscription service from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_subscription_service_init(UmiContextSubscriptionService *state)
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
 * Provide the context subscription service set field operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_subscription_service_set_field(UmiContextSubscriptionService *state,size_t field_index,const char *value)
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
    case 0U: target = state->operation_id;
capacity = sizeof(state->operation_id);
break;
    case 1U: target = state->subscription_id;
capacity = sizeof(state->subscription_id);
break;
    case 2U: target = state->channel_id;
capacity = sizeof(state->channel_id);
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
 * Provide the context subscription service field operation used by this module and its
 * client applications.
 */
const char *umi_context_subscription_service_field(const UmiContextSubscriptionService *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->operation_id;
    case 1U: return state->subscription_id;
    case 2U: return state->channel_id;
    case 3U: return state->panel_id;
    default:return NULL;
    
}
}
/*
 * Provide the context subscription service record success operation used by this module
 * and its client applications.
 */
UmiStatus umi_context_subscription_service_record_success(UmiContextSubscriptionService *state,uint64_t sequence)
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
 * Provide the context subscription service record failure operation used by this module
 * and its client applications.
 */
UmiStatus umi_context_subscription_service_record_failure(UmiContextSubscriptionService *state,UmiStatus status,uint64_t sequence)
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
 * Check that context subscription service satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_context_subscription_service_validate(const UmiContextSubscriptionService *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->operation_id, '\0', sizeof(state->operation_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->subscription_id, '\0', sizeof(state->subscription_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->channel_id, '\0', sizeof(state->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->panel_id, '\0', sizeof(state->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->operation_id,sizeof(state->operation_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->subscription_id,sizeof(state->subscription_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->panel_id,sizeof(state->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context subscription service covers sequence operation used by this module
 * and its client applications.
 */
bool umi_context_subscription_service_covers_sequence(const UmiContextSubscriptionService *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextSubscriptionServiceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x42715dd776e69ec3);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSubscriptionService *)0)->operation_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSubscriptionService *)0)->subscription_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSubscriptionService *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSubscriptionService *)0)->panel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextSubscriptionServiceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextSubscriptionService *)0)->operation_id) - 1U +
        8U + sizeof(((UmiContextSubscriptionService *)0)->subscription_id) - 1U +
        8U + sizeof(((UmiContextSubscriptionService *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextSubscriptionService *)0)->panel_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextSubscriptionServiceArchiveWrite(UmiArchiveWriter *writer, const UmiContextSubscriptionService *value)
{
    UmiArchiveWriteText(writer, value->operation_id, sizeof(value->operation_id));
    UmiArchiveWriteText(writer, value->subscription_id, sizeof(value->subscription_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextSubscriptionServiceArchiveRead(UmiArchiveReader *reader, UmiContextSubscriptionService *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->operation_id, sizeof(value->operation_id));
    UmiArchiveReadText(reader, value->subscription_id, sizeof(value->subscription_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextSubscriptionServiceArchiveValidate(const UmiContextSubscriptionService *value)
{
    return umi_context_subscription_service_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_subscription_service_archive_encode, umi_context_subscription_service_archive_decode,
    UmiContextSubscriptionService, UmiContextSubscriptionServiceArchiveSchema, UmiContextSubscriptionServiceArchiveBound, UmiContextSubscriptionServiceArchiveWrite, UmiContextSubscriptionServiceArchiveRead, UmiContextSubscriptionServiceArchiveValidate)
