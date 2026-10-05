/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/host_slot.c
 *
 * PURPOSE:
 *   Implement cross-application panel host slot validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/host_slot.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel host slot from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_host_slot_init(UmiPanelHostSlot *record)
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
/* Check that panel host slot satisfies its contract before another service relies on it. */
UmiStatus umi_panel_host_slot_validate(const UmiPanelHostSlot *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->slot_id, '\0', sizeof(record->slot_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->instance_id, '\0', sizeof(record->instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->container_id, '\0', sizeof(record->container_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->slot_id,sizeof(record->slot_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->container_id,sizeof(record->container_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->slot_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel host slot store from caller-provided values so later operations receive
 * a known state.
 */
void umi_panel_host_slot_store_init(UmiPanelHostSlotStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel host slot store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelHostSlot *umi_panel_host_slot_store_find(UmiPanelHostSlotStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].slot_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel host slot store find const operation used by this module and its
 * client applications.
 */
const UmiPanelHostSlot *umi_panel_host_slot_store_find_const(const UmiPanelHostSlotStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].slot_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel host slot store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_host_slot_store_put(UmiPanelHostSlotStore *store,const UmiPanelHostSlot *record){
UmiPanelHostSlot *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_host_slot_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_host_slot_store_find(store,record->slot_id);
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
 * Remove panel host slot store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_host_slot_store_remove(UmiPanelHostSlotStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].slot_id,identity)==0){
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
 * Provide the panel host slot store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_host_slot_store_snapshot(const UmiPanelHostSlotStore *store,UmiPanelHostSlot *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelHostSlotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xdc64a2ab1d5b28e3);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelHostSlot *)0)->slot_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelHostSlot *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelHostSlot *)0)->container_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelHostSlotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelHostSlot *)0)->slot_id) - 1U +
        8U + sizeof(((UmiPanelHostSlot *)0)->instance_id) - 1U +
        8U + sizeof(((UmiPanelHostSlot *)0)->container_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPanelHostSlotArchiveWrite(UmiArchiveWriter *writer, const UmiPanelHostSlot *value)
{
    UmiArchiveWriteText(writer, value->slot_id, sizeof(value->slot_id));
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->container_id, sizeof(value->container_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->placement);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->order);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelHostSlotArchiveRead(UmiArchiveReader *reader, UmiPanelHostSlot *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->slot_id, sizeof(value->slot_id));
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->container_id, sizeof(value->container_id));
    value->placement = (UmiPanelPlacement)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->order = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelHostSlotArchiveValidate(const UmiPanelHostSlot *value)
{
    return umi_panel_host_slot_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_host_slot_archive_encode, umi_panel_host_slot_archive_decode,
    UmiPanelHostSlot, UmiPanelHostSlotArchiveSchema, UmiPanelHostSlotArchiveBound, UmiPanelHostSlotArchiveWrite, UmiPanelHostSlotArchiveRead, UmiPanelHostSlotArchiveValidate)
