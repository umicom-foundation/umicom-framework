/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_replay.c
 *
 * PURPOSE:
 *   Implement replay retained contexts with a deterministic cursor and delivery budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_replay.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context replay from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_replay_init(UmiContextReplay *state)
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
 * Provide the context replay set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_replay_set_field(UmiContextReplay *state,size_t field_index,const char *value)
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
    case 0U: target = state->channel_id;
capacity = sizeof(state->channel_id);
break;
    case 1U: target = state->from_context_id;
capacity = sizeof(state->from_context_id);
break;
    case 2U: target = state->target_application;
capacity = sizeof(state->target_application);
break;
    case 3U: target = state->target_panel;
capacity = sizeof(state->target_panel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context replay field operation used by this module and its client
 * applications.
 */
const char *umi_context_replay_field(const UmiContextReplay *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->channel_id;
    case 1U: return state->from_context_id;
    case 2U: return state->target_application;
    case 3U: return state->target_panel;
    default:return NULL;
    
}
}
/*
 * Provide the context replay record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_replay_record_success(UmiContextReplay *state,uint64_t sequence)
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
 * Provide the context replay record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_replay_record_failure(UmiContextReplay *state,UmiStatus status,uint64_t sequence)
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
/* Check that context replay satisfies its contract before another service relies on it. */
UmiStatus umi_context_replay_validate(const UmiContextReplay *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->channel_id, '\0', sizeof(state->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->from_context_id, '\0', sizeof(state->from_context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->target_application, '\0', sizeof(state->target_application)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->target_panel, '\0', sizeof(state->target_panel)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->channel_id,sizeof(state->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->from_context_id,sizeof(state->from_context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->target_application,sizeof(state->target_application)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->target_panel,sizeof(state->target_panel)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context replay covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_replay_covers_sequence(const UmiContextReplay *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextReplayArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf383ab37d719a3ee);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextReplay *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextReplay *)0)->from_context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextReplay *)0)->target_application)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextReplay *)0)->target_panel)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextReplayArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextReplay *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextReplay *)0)->from_context_id) - 1U +
        8U + sizeof(((UmiContextReplay *)0)->target_application) - 1U +
        8U + sizeof(((UmiContextReplay *)0)->target_panel) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextReplayArchiveWrite(UmiArchiveWriter *writer, const UmiContextReplay *value)
{
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->from_context_id, sizeof(value->from_context_id));
    UmiArchiveWriteText(writer, value->target_application, sizeof(value->target_application));
    UmiArchiveWriteText(writer, value->target_panel, sizeof(value->target_panel));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextReplayArchiveRead(UmiArchiveReader *reader, UmiContextReplay *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->from_context_id, sizeof(value->from_context_id));
    UmiArchiveReadText(reader, value->target_application, sizeof(value->target_application));
    UmiArchiveReadText(reader, value->target_panel, sizeof(value->target_panel));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextReplayArchiveValidate(const UmiContextReplay *value)
{
    return umi_context_replay_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_replay_archive_encode, umi_context_replay_archive_decode,
    UmiContextReplay, UmiContextReplayArchiveSchema, UmiContextReplayArchiveBound, UmiContextReplayArchiveWrite, UmiContextReplayArchiveRead, UmiContextReplayArchiveValidate)
