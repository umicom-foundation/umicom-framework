/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/recent_context.c
 *
 * PURPOSE:
 *   Implement track recent and pinned context subjects for navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/recent_context.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context recent context from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_recent_context_init(UmiRecentContext *record)
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
 * Check that context recent context satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_recent_context_validate(const UmiRecentContext *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->context_id, '\0', sizeof(record->context_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->channel_id, '\0', sizeof(record->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->schema_id, '\0', sizeof(record->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->label, '\0', sizeof(record->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

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
    if (!umi_context_text_is_valid(record->label, sizeof(record->label))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->context_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context recent context store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_recent_context_store_init(UmiRecentContextStore *store)
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
 * Find context recent context store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiRecentContext *umi_context_recent_context_store_find(UmiRecentContextStore *store,const char *identity)
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
 * Provide the context recent context store find const operation used by this module and
 * its client applications.
 */
const UmiRecentContext *umi_context_recent_context_store_find_const(const UmiRecentContextStore *store,const char *identity)
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
 * Provide the context recent context store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_recent_context_store_put(UmiRecentContextStore *store,const UmiRecentContext *record)
{
    UmiRecentContext *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_recent_context_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_recent_context_store_find(store,record->context_id);
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
    if(store->count>=UMI_CONTEXT_RECENT_CONTEXT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context recent context store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_recent_context_store_remove(UmiRecentContextStore *store,const char *identity)
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
 * Return the number of records represented by context recent context store without
 * changing their state.
 */
size_t umi_context_recent_context_store_count(const UmiRecentContextStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context recent context store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_recent_context_store_snapshot(const UmiRecentContextStore *store,UmiRecentContext *out_records,size_t capacity,size_t *out_count)
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
static uint64_t UmiRecentContextArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x1d2419272e005d9b);
    schema = (schema ^ (uint64_t)sizeof(((UmiRecentContext *)0)->context_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRecentContext *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRecentContext *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiRecentContext *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiRecentContextArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiRecentContext *)0)->context_id) - 1U +
        8U + sizeof(((UmiRecentContext *)0)->channel_id) - 1U +
        8U + sizeof(((UmiRecentContext *)0)->schema_id) - 1U +
        8U + sizeof(((UmiRecentContext *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiRecentContextArchiveWrite(UmiArchiveWriter *writer, const UmiRecentContext *value)
{
    UmiArchiveWriteText(writer, value->context_id, sizeof(value->context_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_used_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->pinned);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiRecentContextArchiveRead(UmiArchiveReader *reader, UmiRecentContext *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->context_id, sizeof(value->context_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->last_used_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->pinned = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiRecentContextArchiveValidate(const UmiRecentContext *value)
{
    return umi_context_recent_context_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_recent_context_archive_encode, umi_context_recent_context_archive_decode,
    UmiRecentContext, UmiRecentContextArchiveSchema, UmiRecentContextArchiveBound, UmiRecentContextArchiveWrite, UmiRecentContextArchiveRead, UmiRecentContextArchiveValidate)
