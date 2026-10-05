/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/permission.c
 *
 * PURPOSE:
 *   Implement cross-application panel permission validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/permission.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel permission from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_permission_init(UmiPanelPermission *record)
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
/* Check that panel permission satisfies its contract before another service relies on it. */
UmiStatus umi_panel_permission_validate(const UmiPanelPermission *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->permission_id, '\0', sizeof(record->permission_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subject_id, '\0', sizeof(record->subject_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->permission_id,sizeof(record->permission_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->subject_id,sizeof(record->subject_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->permission_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel permission store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_permission_store_init(UmiPanelPermissionStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel permission store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelPermission *umi_panel_permission_store_find(UmiPanelPermissionStore *store,const char *identity){
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
 * Provide the panel permission store find const operation used by this module and its
 * client applications.
 */
const UmiPanelPermission *umi_panel_permission_store_find_const(const UmiPanelPermissionStore *store,const char *identity){
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
 * Provide the panel permission store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_permission_store_put(UmiPanelPermissionStore *store,const UmiPanelPermission *record){
UmiPanelPermission *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_permission_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_permission_store_find(store,record->permission_id);
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(existing!=NULL){
next=existing->revision+1U;
*existing=*record;
existing->revision=next;
store->revision+=1U;
return UMI_STATUS_OK;
}
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(store->count>=UMI_PANEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove panel permission store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_permission_store_remove(UmiPanelPermissionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].permission_id,identity)==0){
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
return UMI_STATUS_NOT_FOUND;
}
/*
 * Provide the panel permission store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_permission_store_snapshot(const UmiPanelPermissionStore *store,UmiPanelPermission *records,size_t capacity,size_t *out_count){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store->count>capacity||(store->count!=0U&&records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(store->count!=0U)memcpy(records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiPanelPermissionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd4c99d531bb1cf3d);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelPermission *)0)->permission_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelPermission *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelPermission *)0)->subject_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelPermissionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelPermission *)0)->permission_id) - 1U +
        8U + sizeof(((UmiPanelPermission *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelPermission *)0)->subject_id) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPanelPermissionArchiveWrite(UmiArchiveWriter *writer, const UmiPanelPermission *value)
{
    UmiArchiveWriteText(writer, value->permission_id, sizeof(value->permission_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->subject_id, sizeof(value->subject_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_open);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_close);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_move);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_rebind);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->allow_cross_application);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelPermissionArchiveRead(UmiArchiveReader *reader, UmiPanelPermission *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->permission_id, sizeof(value->permission_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->subject_id, sizeof(value->subject_id));
    value->allow_open = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_close = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_move = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_rebind = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->allow_cross_application = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelPermissionArchiveValidate(const UmiPanelPermission *value)
{
    return umi_panel_permission_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_permission_archive_encode, umi_panel_permission_archive_decode,
    UmiPanelPermission, UmiPanelPermissionArchiveSchema, UmiPanelPermissionArchiveBound, UmiPanelPermissionArchiveWrite, UmiPanelPermissionArchiveRead, UmiPanelPermissionArchiveValidate)
