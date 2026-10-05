/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/permission.c
 *
 * PURPOSE:
 *   Implement represent user and workload permissions for context operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/permission.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context permission from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_permission_init(UmiContextPermission *record)
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
 * Check that context permission satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_permission_validate(const UmiContextPermission *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->permission_id, '\0', sizeof(record->permission_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->channel_id, '\0', sizeof(record->channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->permission_id, sizeof(record->permission_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->subject_id, sizeof(record->subject_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->channel_id, sizeof(record->channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->permission_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context permission store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_permission_store_init(UmiContextPermissionStore *store)
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
 * Find context permission store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextPermission *umi_context_permission_store_find(UmiContextPermissionStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].permission_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context permission store find const operation used by this module and its
 * client applications.
 */
const UmiContextPermission *umi_context_permission_store_find_const(const UmiContextPermissionStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].permission_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context permission store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_permission_store_put(UmiContextPermissionStore *store,const UmiContextPermission *record)
{
    UmiContextPermission *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_permission_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_permission_store_find(store,record->permission_id);
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
    if(store->count>=UMI_CONTEXT_PERMISSION_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context permission store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_permission_store_remove(UmiContextPermissionStore *store,const char *identity)
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
if(strcmp(store->items[i].permission_id,identity)==0){
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
 * Return the number of records represented by context permission store without changing
 * their state.
 */
size_t umi_context_permission_store_count(const UmiContextPermissionStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context permission store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_permission_store_snapshot(const UmiContextPermissionStore *store,UmiContextPermission *out_records,size_t capacity,size_t *out_count)
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
static uint64_t UmiContextPermissionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x4e5cd30da899d4f0);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPermission *)0)->permission_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPermission *)0)->subject_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPermission *)0)->channel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextPermissionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextPermission *)0)->permission_id) - 1U +
        8U + sizeof(((UmiContextPermission *)0)->subject_id) - 1U +
        8U + sizeof(((UmiContextPermission *)0)->channel_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextPermissionArchiveWrite(UmiArchiveWriter *writer, const UmiContextPermission *value)
{
    UmiArchiveWriteText(writer, value->permission_id, sizeof(value->permission_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteText(writer, value->channel_id, sizeof(value->channel_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_publish);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_observe);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_rebind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_history);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextPermissionArchiveRead(UmiArchiveReader *reader, UmiContextPermission *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->permission_id, sizeof(value->permission_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    UmiArchiveReadText(reader, value->channel_id, sizeof(value->channel_id));
    value->allow_publish = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_observe = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_rebind = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_history = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextPermissionArchiveValidate(const UmiContextPermission *value)
{
    return umi_context_permission_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_permission_archive_encode, umi_context_permission_archive_decode,
    UmiContextPermission, UmiContextPermissionArchiveSchema, UmiContextPermissionArchiveBound, UmiContextPermissionArchiveWrite, UmiContextPermissionArchiveRead, UmiContextPermissionArchiveValidate)
