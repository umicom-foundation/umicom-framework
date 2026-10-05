/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/schema.c
 *
 * PURPOSE:
 *   Implement register typed context schemas and compatibility metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/schema.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context schema from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_schema_init(UmiContextSchema *record)
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
/* Check that context schema satisfies its contract before another service relies on it. */
UmiStatus umi_context_schema_validate(const UmiContextSchema *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->schema_id, '\0', sizeof(record->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->display_name, '\0', sizeof(record->display_name)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->description, '\0', sizeof(record->description)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->schema_id, sizeof(record->schema_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->display_name, sizeof(record->display_name))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->description, sizeof(record->description))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->schema_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context schema store from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_schema_store_init(UmiContextSchemaStore *store)
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
 * Find context schema store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiContextSchema *umi_context_schema_store_find(UmiContextSchemaStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].schema_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context schema store find const operation used by this module and its client
 * applications.
 */
const UmiContextSchema *umi_context_schema_store_find_const(const UmiContextSchemaStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].schema_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context schema store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_schema_store_put(UmiContextSchemaStore *store,const UmiContextSchema *record)
{
    UmiContextSchema *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_schema_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_schema_store_find(store,record->schema_id);
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
    if(store->count>=UMI_CONTEXT_SCHEMA_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context schema store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_schema_store_remove(UmiContextSchemaStore *store,const char *identity)
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
if(strcmp(store->items[i].schema_id,identity)==0){
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
 * Return the number of records represented by context schema store without changing their
 * state.
 */
size_t umi_context_schema_store_count(const UmiContextSchemaStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context schema store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_schema_store_snapshot(const UmiContextSchemaStore *store,UmiContextSchema *out_records,size_t capacity,size_t *out_count)
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
static uint64_t UmiContextSchemaArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x6691ede005efa846);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchema *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchema *)0)->display_name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextSchema *)0)->description)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextSchemaArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextSchema *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextSchema *)0)->display_name) - 1U +
        8U + sizeof(((UmiContextSchema *)0)->description) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextSchemaArchiveWrite(UmiArchiveWriter *writer, const UmiContextSchema *value)
{
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->display_name, sizeof(value->display_name));
    UmiArchiveWriteText(writer, value->description, sizeof(value->description));
    UmiArchiveWriteSigned(writer, (int64_t)value->kind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->schema_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_compatible_version);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sensitive);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextSchemaArchiveRead(UmiArchiveReader *reader, UmiContextSchema *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->display_name, sizeof(value->display_name));
    UmiArchiveReadText(reader, value->description, sizeof(value->description));
    value->kind = (UmiContextKind)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->schema_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->minimum_compatible_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->sensitive = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextSchemaArchiveValidate(const UmiContextSchema *value)
{
    return umi_context_schema_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_schema_archive_encode, umi_context_schema_archive_decode,
    UmiContextSchema, UmiContextSchemaArchiveSchema, UmiContextSchemaArchiveBound, UmiContextSchemaArchiveWrite, UmiContextSchemaArchiveRead, UmiContextSchemaArchiveValidate)
