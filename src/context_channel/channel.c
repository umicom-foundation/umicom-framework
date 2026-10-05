/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/channel.c
 *
 * PURPOSE:
 *   Implement define named typed channels and their presentation colour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/channel.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context channel from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_channel_init(UmiContextChannel *record)
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
/* Check that context channel satisfies its contract before another service relies on it. */
UmiStatus umi_context_channel_validate(const UmiContextChannel *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->channel_id, '\0', sizeof(record->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->display_name, '\0', sizeof(record->display_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->schema_id, '\0', sizeof(record->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->channel_id, sizeof(record->channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->display_name, sizeof(record->display_name))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->schema_id, sizeof(record->schema_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->channel_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context channel store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_channel_store_init(UmiContextChannelStore *store)
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
 * Find context channel store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextChannel *umi_context_channel_store_find(UmiContextChannelStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].channel_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context channel store find const operation used by this module and its
 * client applications.
 */
const UmiContextChannel *umi_context_channel_store_find_const(const UmiContextChannelStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].channel_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context channel store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_channel_store_put(UmiContextChannelStore *store,const UmiContextChannel *record)
{
    UmiContextChannel *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_channel_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_channel_store_find(store,record->channel_id);
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
    if(store->count>=UMI_CONTEXT_CHANNEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context channel store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_channel_store_remove(UmiContextChannelStore *store,const char *identity)
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
if(strcmp(store->items[i].channel_id,identity)==0){
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
 * Return the number of records represented by context channel store without changing their
 * state.
 */
size_t umi_context_channel_store_count(const UmiContextChannelStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context channel store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_channel_store_snapshot(const UmiContextChannelStore *store,UmiContextChannel *out_records,size_t capacity,size_t *out_count)
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
static uint64_t UmiContextChannelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa86bb92e690cd144);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextChannel *)0)->channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextChannel *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextChannel *)0)->schema_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextChannelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextChannel *)0)->channel_id) - 1U +
        8U + sizeof(((UmiContextChannel *)0)->display_name) - 1U +
        8U + sizeof(((UmiContextChannel *)0)->schema_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextChannelArchiveWrite(UmiArchiveWriter *writer, const UmiContextChannel *value)
{
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteSigned(writer, (int64_t)value->colour);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->persistent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->last_sequence);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextChannelArchiveRead(UmiArchiveReader *reader, UmiContextChannel *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    value->kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->colour = (UmiContextChannelColour)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->persistent = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->last_sequence = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextChannelArchiveValidate(const UmiContextChannel *value)
{
    return umi_context_channel_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_channel_archive_encode, umi_context_channel_archive_decode,
    UmiContextChannel, UmiContextChannelArchiveSchema, UmiContextChannelArchiveBound, UmiContextChannelArchiveWrite, UmiContextChannelArchiveRead, UmiContextChannelArchiveValidate)
