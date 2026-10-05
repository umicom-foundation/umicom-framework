/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_recovery.c
 *
 * PURPOSE:
 *   Implement retain context recovery checkpoints after application or process failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_recovery.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context recovery from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_recovery_init(UmiContextRecovery *state)
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
 * Provide the context recovery set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_recovery_set_field(UmiContextRecovery *state,size_t field_index,const char *value)
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
    case 0U: target = state->recovery_id;
capacity = sizeof(state->recovery_id);
break;
    case 1U: target = state->session_id;
capacity = sizeof(state->session_id);
break;
    case 2U: target = state->context_id;
capacity = sizeof(state->context_id);
break;
    case 3U: target = state->checkpoint_id;
capacity = sizeof(state->checkpoint_id);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context recovery field operation used by this module and its client
 * applications.
 */
const char *umi_context_recovery_field(const UmiContextRecovery *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->recovery_id;
    case 1U: return state->session_id;
    case 2U: return state->context_id;
    case 3U: return state->checkpoint_id;
    default:return NULL;
    
}
}
/*
 * Provide the context recovery record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_recovery_record_success(UmiContextRecovery *state,uint64_t sequence)
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
 * Provide the context recovery record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_recovery_record_failure(UmiContextRecovery *state,UmiStatus status,uint64_t sequence)
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
/* Check that context recovery satisfies its contract before another service relies on it. */
UmiStatus umi_context_recovery_validate(const UmiContextRecovery *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->recovery_id, '\0', sizeof(state->recovery_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->session_id, '\0', sizeof(state->session_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->context_id, '\0', sizeof(state->context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->checkpoint_id, '\0', sizeof(state->checkpoint_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->recovery_id,sizeof(state->recovery_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->session_id,sizeof(state->session_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->context_id,sizeof(state->context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->checkpoint_id,sizeof(state->checkpoint_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context recovery covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_recovery_covers_sequence(const UmiContextRecovery *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextRecoveryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x924615f7d84b68c4);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextRecovery *)0)->recovery_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextRecovery *)0)->session_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextRecovery *)0)->context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextRecovery *)0)->checkpoint_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextRecoveryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextRecovery *)0)->recovery_id) - 1U +
        8U + sizeof(((UmiContextRecovery *)0)->session_id) - 1U +
        8U + sizeof(((UmiContextRecovery *)0)->context_id) - 1U +
        8U + sizeof(((UmiContextRecovery *)0)->checkpoint_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextRecoveryArchiveWrite(UmiArchiveWriter *writer, const UmiContextRecovery *value)
{
    UmiArchiveWriteText(writer, value->recovery_id, sizeof(value->recovery_id));
    UmiArchiveWriteText(writer, value->session_id, sizeof(value->session_id));
    UmiArchiveWriteText(writer, value->context_id, sizeof(value->context_id));
    UmiArchiveWriteText(writer, value->checkpoint_id, sizeof(value->checkpoint_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextRecoveryArchiveRead(UmiArchiveReader *reader, UmiContextRecovery *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->recovery_id, sizeof(value->recovery_id));
    UmiArchiveReadText(reader, value->session_id, sizeof(value->session_id));
    UmiArchiveReadText(reader, value->context_id, sizeof(value->context_id));
    UmiArchiveReadText(reader, value->checkpoint_id, sizeof(value->checkpoint_id));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextRecoveryArchiveValidate(const UmiContextRecovery *value)
{
    return umi_context_recovery_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_recovery_archive_encode, umi_context_recovery_archive_decode,
    UmiContextRecovery, UmiContextRecoveryArchiveSchema, UmiContextRecoveryArchiveBound, UmiContextRecoveryArchiveWrite, UmiContextRecoveryArchiveRead, UmiContextRecoveryArchiveValidate)
