/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/audit_record.c
 *
 * PURPOSE:
 *   Implement retain security and operational context audit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/audit_record.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context audit record from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_audit_record_init(UmiContextAuditRecord *record)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
/*
 * Check that context audit record satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_audit_record_validate(const UmiContextAuditRecord *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->audit_id, '\0', sizeof(record->audit_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->actor_id, '\0', sizeof(record->actor_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->action_id, '\0', sizeof(record->action_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->channel_id, '\0', sizeof(record->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->context_id, '\0', sizeof(record->context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->target_id, '\0', sizeof(record->target_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->audit_id, sizeof(record->audit_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->actor_id, sizeof(record->actor_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->action_id, sizeof(record->action_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->channel_id, sizeof(record->channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->context_id, sizeof(record->context_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->target_id, sizeof(record->target_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->audit_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context audit record store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_audit_record_store_init(UmiContextAuditRecordStore *store)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find context audit record store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextAuditRecord *umi_context_audit_record_store_find(UmiContextAuditRecordStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].audit_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context audit record store find const operation used by this module and its
 * client applications.
 */
const UmiContextAuditRecord *umi_context_audit_record_store_find_const(const UmiContextAuditRecordStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].audit_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context audit record store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_audit_record_store_put(UmiContextAuditRecordStore *store,const UmiContextAuditRecord *record)
{
    UmiContextAuditRecord *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_audit_record_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_audit_record_store_find(store,record->audit_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(store->count>=UMI_CONTEXT_AUDIT_RECORD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context audit record store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_audit_record_store_remove(UmiContextAuditRecordStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i){
/* Use the stable identifier comparison to choose the matching record or policy. */
if(strcmp(store->items[i].audit_id,identity)==0){
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by context audit record store without changing
 * their state.
 */
size_t umi_context_audit_record_store_count(const UmiContextAuditRecordStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context audit record store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_audit_record_store_snapshot(const UmiContextAuditRecordStore *store,UmiContextAuditRecord *out_records,size_t capacity,size_t *out_count)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextAuditRecordArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xedb541b9a61081f2);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextAuditRecord *)0)->audit_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextAuditRecord *)0)->actor_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextAuditRecord *)0)->action_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextAuditRecord *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextAuditRecord *)0)->context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextAuditRecord *)0)->target_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextAuditRecordArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextAuditRecord *)0)->audit_id) - 1U +
        8U + sizeof(((UmiContextAuditRecord *)0)->actor_id) - 1U +
        8U + sizeof(((UmiContextAuditRecord *)0)->action_id) - 1U +
        8U + sizeof(((UmiContextAuditRecord *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextAuditRecord *)0)->context_id) - 1U +
        8U + sizeof(((UmiContextAuditRecord *)0)->target_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiContextAuditRecordArchiveWrite(UmiArchiveWriter *writer, const UmiContextAuditRecord *value)
{
    UmiArchiveWriteText(writer, value->audit_id, sizeof(value->audit_id));
    UmiArchiveWriteText(writer, value->actor_id, sizeof(value->actor_id));
    UmiArchiveWriteText(writer, value->action_id, sizeof(value->action_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->context_id, sizeof(value->context_id));
    UmiArchiveWriteText(writer, value->target_id, sizeof(value->target_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextAuditRecordArchiveRead(UmiArchiveReader *reader, UmiContextAuditRecord *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->audit_id, sizeof(value->audit_id));
    UmiArchiveReadText(reader, value->actor_id, sizeof(value->actor_id));
    UmiArchiveReadText(reader, value->action_id, sizeof(value->action_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->context_id, sizeof(value->context_id));
    UmiArchiveReadText(reader, value->target_id, sizeof(value->target_id));
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextAuditRecordArchiveValidate(const UmiContextAuditRecord *value)
{
    return umi_context_audit_record_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_audit_record_archive_encode, umi_context_audit_record_archive_decode,
    UmiContextAuditRecord, UmiContextAuditRecordArchiveSchema, UmiContextAuditRecordArchiveBound, UmiContextAuditRecordArchiveWrite, UmiContextAuditRecordArchiveRead, UmiContextAuditRecordArchiveValidate)
