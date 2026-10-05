/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_inspector.c
 *
 * PURPOSE:
 *   Implement expose a read-only property projection for one typed context payload.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_inspector.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context inspector from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_inspector_init(UmiContextInspector *state)
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
 * Provide the context inspector set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_inspector_set_field(UmiContextInspector *state,size_t field_index,const char *value)
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
    case 0U: target = state->context_id;
capacity = sizeof(state->context_id);
break;
    case 1U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 2U: target = state->source_application;
capacity = sizeof(state->source_application);
break;
    case 3U: target = state->source_panel;
capacity = sizeof(state->source_panel);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context inspector field operation used by this module and its client
 * applications.
 */
const char *umi_context_inspector_field(const UmiContextInspector *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->context_id;
    case 1U: return state->schema_id;
    case 2U: return state->source_application;
    case 3U: return state->source_panel;
    default:return NULL;
    
}
}
/*
 * Provide the context inspector record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_inspector_record_success(UmiContextInspector *state,uint64_t sequence)
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
 * Provide the context inspector record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_inspector_record_failure(UmiContextInspector *state,UmiStatus status,uint64_t sequence)
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
/* Check that context inspector satisfies its contract before another service relies on it. */
UmiStatus umi_context_inspector_validate(const UmiContextInspector *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->context_id, '\0', sizeof(state->context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->schema_id, '\0', sizeof(state->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->source_application, '\0', sizeof(state->source_application)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->source_panel, '\0', sizeof(state->source_panel)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->context_id,sizeof(state->context_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->source_application,sizeof(state->source_application)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->source_panel,sizeof(state->source_panel)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context inspector covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_inspector_covers_sequence(const UmiContextInspector *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextInspectorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4a3a582d124ad1e7);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextInspector *)0)->context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextInspector *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextInspector *)0)->source_application)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextInspector *)0)->source_panel)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextInspectorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextInspector *)0)->context_id) - 1U +
        8U + sizeof(((UmiContextInspector *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextInspector *)0)->source_application) - 1U +
        8U + sizeof(((UmiContextInspector *)0)->source_panel) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextInspectorArchiveWrite(UmiArchiveWriter *writer, const UmiContextInspector *value)
{
    UmiArchiveWriteText(writer, value->context_id, sizeof(value->context_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->source_application, sizeof(value->source_application));
    UmiArchiveWriteText(writer, value->source_panel, sizeof(value->source_panel));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextInspectorArchiveRead(UmiArchiveReader *reader, UmiContextInspector *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->context_id, sizeof(value->context_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->source_application, sizeof(value->source_application));
    UmiArchiveReadText(reader, value->source_panel, sizeof(value->source_panel));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextInspectorArchiveValidate(const UmiContextInspector *value)
{
    return umi_context_inspector_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_inspector_archive_encode, umi_context_inspector_archive_decode,
    UmiContextInspector, UmiContextInspectorArchiveSchema, UmiContextInspectorArchiveBound, UmiContextInspectorArchiveWrite, UmiContextInspectorArchiveRead, UmiContextInspectorArchiveValidate)
