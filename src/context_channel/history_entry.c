/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/history_entry.c
 *
 * PURPOSE:
 *   Implement retain bounded delivery history and evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/history_entry.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context history entry from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_history_entry_init(UmiContextHistoryEntry *record)
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
 * Check that context history entry satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_history_entry_validate(const UmiContextHistoryEntry *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->context_id, '\0', sizeof(record->context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->channel_id, '\0', sizeof(record->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->schema_id, '\0', sizeof(record->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_application_id, '\0', sizeof(record->source_application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_panel_id, '\0', sizeof(record->source_panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->context_id, sizeof(record->context_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->channel_id, sizeof(record->channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->schema_id, sizeof(record->schema_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->source_application_id, sizeof(record->source_application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->source_panel_id, sizeof(record->source_panel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->context_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context history entry store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_history_entry_store_init(UmiContextHistoryEntryStore *store)
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
 * Find context history entry store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiContextHistoryEntry *umi_context_history_entry_store_find(UmiContextHistoryEntryStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].context_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context history entry store find const operation used by this module and its
 * client applications.
 */
const UmiContextHistoryEntry *umi_context_history_entry_store_find_const(const UmiContextHistoryEntryStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].context_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context history entry store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_history_entry_store_put(UmiContextHistoryEntryStore *store,const UmiContextHistoryEntry *record)
{
    UmiContextHistoryEntry *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_history_entry_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_history_entry_store_find(store,record->context_id);
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
    if(store->count>=UMI_CONTEXT_HISTORY_ENTRY_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context history entry store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_history_entry_store_remove(UmiContextHistoryEntryStore *store,const char *identity)
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
if(strcmp(store->items[i].context_id,identity)==0){
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
 * Return the number of records represented by context history entry store without changing
 * their state.
 */
size_t umi_context_history_entry_store_count(const UmiContextHistoryEntryStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context history entry store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_history_entry_store_snapshot(const UmiContextHistoryEntryStore *store,UmiContextHistoryEntry *out_records,size_t capacity,size_t *out_count)
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
static uint64_t UmiContextHistoryEntryArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x74d3f5b502c8c23b);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHistoryEntry *)0)->context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHistoryEntry *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHistoryEntry *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHistoryEntry *)0)->source_application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextHistoryEntry *)0)->source_panel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextHistoryEntryArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextHistoryEntry *)0)->context_id) - 1U +
        8U + sizeof(((UmiContextHistoryEntry *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextHistoryEntry *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextHistoryEntry *)0)->source_application_id) - 1U +
        8U + sizeof(((UmiContextHistoryEntry *)0)->source_panel_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextHistoryEntryArchiveWrite(UmiArchiveWriter *writer, const UmiContextHistoryEntry *value)
{
    UmiArchiveWriteText(writer, value->context_id, sizeof(value->context_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->source_application_id, sizeof(value->source_application_id));
    UmiArchiveWriteText(writer, value->source_panel_id, sizeof(value->source_panel_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->content_hash);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextHistoryEntryArchiveRead(UmiArchiveReader *reader, UmiContextHistoryEntry *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->context_id, sizeof(value->context_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->source_application_id, sizeof(value->source_application_id));
    UmiArchiveReadText(reader, value->source_panel_id, sizeof(value->source_panel_id));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->content_hash = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->state = (UmiContextDeliveryState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextHistoryEntryArchiveValidate(const UmiContextHistoryEntry *value)
{
    return umi_context_history_entry_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_history_entry_archive_encode, umi_context_history_entry_archive_decode,
    UmiContextHistoryEntry, UmiContextHistoryEntryArchiveSchema, UmiContextHistoryEntryArchiveBound, UmiContextHistoryEntryArchiveWrite, UmiContextHistoryEntryArchiveRead, UmiContextHistoryEntryArchiveValidate)
