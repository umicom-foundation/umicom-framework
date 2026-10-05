/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_preference.c
 *
 * PURPOSE:
 *   Implement store user-facing context-link preferences independently of gtk widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_preference.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context preference from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_preference_init(UmiContextPreference *state)
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
 * Provide the context preference set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_preference_set_field(UmiContextPreference *state,size_t field_index,const char *value)
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
    case 0U: target = state->preference_id;
capacity = sizeof(state->preference_id);
break;
    case 1U: target = state->user_id;
capacity = sizeof(state->user_id);
break;
    case 2U: target = state->default_colour;
capacity = sizeof(state->default_colour);
break;
    case 3U: target = state->default_channel;
capacity = sizeof(state->default_channel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context preference field operation used by this module and its client
 * applications.
 */
const char *umi_context_preference_field(const UmiContextPreference *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->preference_id;
    case 1U: return state->user_id;
    case 2U: return state->default_colour;
    case 3U: return state->default_channel;
    default:return NULL;
    
}
}
/*
 * Provide the context preference record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_preference_record_success(UmiContextPreference *state,uint64_t sequence)
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
 * Provide the context preference record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_preference_record_failure(UmiContextPreference *state,UmiStatus status,uint64_t sequence)
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
 * Check that context preference satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_preference_validate(const UmiContextPreference *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->preference_id, '\0', sizeof(state->preference_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->user_id, '\0', sizeof(state->user_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->default_colour, '\0', sizeof(state->default_colour)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->default_channel, '\0', sizeof(state->default_channel)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->preference_id,sizeof(state->preference_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->user_id,sizeof(state->user_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->default_colour,sizeof(state->default_colour)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->default_channel,sizeof(state->default_channel)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context preference covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_preference_covers_sequence(const UmiContextPreference *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextPreferenceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4a1fa5a47da5cbbd);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPreference *)0)->preference_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPreference *)0)->user_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPreference *)0)->default_colour)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPreference *)0)->default_channel)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextPreferenceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextPreference *)0)->preference_id) - 1U +
        8U + sizeof(((UmiContextPreference *)0)->user_id) - 1U +
        8U + sizeof(((UmiContextPreference *)0)->default_colour) - 1U +
        8U + sizeof(((UmiContextPreference *)0)->default_channel) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextPreferenceArchiveWrite(UmiArchiveWriter *writer, const UmiContextPreference *value)
{
    UmiArchiveWriteText(writer, value->preference_id, sizeof(value->preference_id));
    UmiArchiveWriteText(writer, value->user_id, sizeof(value->user_id));
    UmiArchiveWriteText(writer, value->default_colour, sizeof(value->default_colour));
    UmiArchiveWriteText(writer, value->default_channel, sizeof(value->default_channel));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextPreferenceArchiveRead(UmiArchiveReader *reader, UmiContextPreference *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->preference_id, sizeof(value->preference_id));
    UmiArchiveReadText(reader, value->user_id, sizeof(value->user_id));
    UmiArchiveReadText(reader, value->default_colour, sizeof(value->default_colour));
    UmiArchiveReadText(reader, value->default_channel, sizeof(value->default_channel));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextPreferenceArchiveValidate(const UmiContextPreference *value)
{
    return umi_context_preference_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_preference_archive_encode, umi_context_preference_archive_decode,
    UmiContextPreference, UmiContextPreferenceArchiveSchema, UmiContextPreferenceArchiveBound, UmiContextPreferenceArchiveWrite, UmiContextPreferenceArchiveRead, UmiContextPreferenceArchiveValidate)
