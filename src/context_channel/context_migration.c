/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/context_migration.c
 *
 * PURPOSE:
 *   Implement record schema migration plans for persisted context records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/context_migration.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context migration from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_migration_init(UmiContextMigration *state)
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
 * Provide the context migration set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_migration_set_field(UmiContextMigration *state,size_t field_index,const char *value)
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
    case 0U: target = state->migration_id;
capacity = sizeof(state->migration_id);
break;
    case 1U: target = state->schema_id;
capacity = sizeof(state->schema_id);
break;
    case 2U: target = state->from_version;
capacity = sizeof(state->from_version);
break;
    case 3U: target = state->to_version;
capacity = sizeof(state->to_version);
break;
    default:return UMI_STATUS_NOT_FOUND;
    
}
    status=umi_context_copy_text(target,capacity,value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(status==UMI_STATUS_OK)state->revision+=1U;
    return status;
}
/*
 * Provide the context migration field operation used by this module and its client
 * applications.
 */
const char *umi_context_migration_field(const UmiContextMigration *state,size_t field_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL)return NULL;
    /* Select the behaviour associated with the requested command or state value. */
    switch(field_index){
    case 0U: return state->migration_id;
    case 1U: return state->schema_id;
    case 2U: return state->from_version;
    case 3U: return state->to_version;
    default:return NULL;
    
}
}
/*
 * Provide the context migration record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_migration_record_success(UmiContextMigration *state,uint64_t sequence)
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
 * Provide the context migration record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_migration_record_failure(UmiContextMigration *state,UmiStatus status,uint64_t sequence)
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
/* Check that context migration satisfies its contract before another service relies on it. */
UmiStatus umi_context_migration_validate(const UmiContextMigration *state)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->migration_id, '\0', sizeof(state->migration_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->schema_id, '\0', sizeof(state->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->from_version, '\0', sizeof(state->from_version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(state->to_version, '\0', sizeof(state->to_version)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(state==NULL||state->structure_size!=sizeof(*state))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->migration_id,sizeof(state->migration_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->schema_id,sizeof(state->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->from_version,sizeof(state->from_version)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(state->to_version,sizeof(state->to_version)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->item_count!=0U&&state->first_sequence>state->last_sequence)return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(state->failure_count>state->item_count)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
/*
 * Provide the context migration covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_migration_covers_sequence(const UmiContextMigration *state,uint64_t sequence)
{
    return state!=NULL&&state->item_count!=0U&&sequence>=state->first_sequence&&sequence<=state->last_sequence;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextMigrationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7f436161dc654394);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMigration *)0)->migration_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMigration *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMigration *)0)->from_version)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextMigration *)0)->to_version)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextMigrationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextMigration *)0)->migration_id) - 1U +
        8U + sizeof(((UmiContextMigration *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextMigration *)0)->from_version) - 1U +
        8U + sizeof(((UmiContextMigration *)0)->to_version) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextMigrationArchiveWrite(UmiArchiveWriter *writer, const UmiContextMigration *value)
{
    UmiArchiveWriteText(writer, value->migration_id, sizeof(value->migration_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->from_version, sizeof(value->from_version));
    UmiArchiveWriteText(writer, value->to_version, sizeof(value->to_version));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->first_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->item_count);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->failure_count);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextMigrationArchiveRead(UmiArchiveReader *reader, UmiContextMigration *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->migration_id, sizeof(value->migration_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->from_version, sizeof(value->from_version));
    UmiArchiveReadText(reader, value->to_version, sizeof(value->to_version));
    value->first_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->item_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->failure_count = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextMigrationArchiveValidate(const UmiContextMigration *value)
{
    return umi_context_migration_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_migration_archive_encode, umi_context_migration_archive_decode,
    UmiContextMigration, UmiContextMigrationArchiveSchema, UmiContextMigrationArchiveBound, UmiContextMigrationArchiveWrite, UmiContextMigrationArchiveRead, UmiContextMigrationArchiveValidate)
