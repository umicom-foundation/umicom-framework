/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/view.c
 *
 * PURPOSE:
 *   Implement cross-application panel view validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/view.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel view from caller-provided values so later operations receive a known
 * state.
 */
void umi_panel_view_init(UmiPanelView *record)
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
/* Check that panel view satisfies its contract before another service relies on it. */
UmiStatus umi_panel_view_validate(const UmiPanelView *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->view_id, '\0', sizeof(record->view_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->instance_id, '\0', sizeof(record->instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->title, '\0', sizeof(record->title)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->subtitle, '\0', sizeof(record->subtitle)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->empty_message, '\0', sizeof(record->empty_message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->view_id,sizeof(record->view_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->title,sizeof(record->title)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->subtitle,sizeof(record->subtitle)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->empty_message,sizeof(record->empty_message)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->view_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel view store from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_view_store_init(UmiPanelViewStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel view store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelView *umi_panel_view_store_find(UmiPanelViewStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].view_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel view store find const operation used by this module and its client
 * applications.
 */
const UmiPanelView *umi_panel_view_store_find_const(const UmiPanelViewStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].view_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel view store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_view_store_put(UmiPanelViewStore *store,const UmiPanelView *record){
UmiPanelView *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_view_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_view_store_find(store,record->view_id);
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
 * Remove panel view store while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_panel_view_store_remove(UmiPanelViewStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].view_id,identity)==0){
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
 * Provide the panel view store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_view_store_snapshot(const UmiPanelViewStore *store,UmiPanelView *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelViewArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x56af20137004795d);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelView *)0)->view_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelView *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelView *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelView *)0)->title)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelView *)0)->subtitle)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelView *)0)->empty_message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelViewArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelView *)0)->view_id) - 1U +
        8U + sizeof(((UmiPanelView *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelView *)0)->instance_id) - 1U +
        8U + sizeof(((UmiPanelView *)0)->title) - 1U +
        8U + sizeof(((UmiPanelView *)0)->subtitle) - 1U +
        8U + sizeof(((UmiPanelView *)0)->empty_message) - 1U +
        8U +
        8U +
        8U;
}
static void UmiPanelViewArchiveWrite(UmiArchiveWriter *writer, const UmiPanelView *value)
{
    UmiArchiveWriteText(writer, value->view_id, sizeof(value->view_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteText(writer, value->subtitle, sizeof(value->subtitle));
    UmiArchiveWriteText(writer, value->empty_message, sizeof(value->empty_message));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->visible);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelViewArchiveRead(UmiArchiveReader *reader, UmiPanelView *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->view_id, sizeof(value->view_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    UmiArchiveReadText(reader, value->subtitle, sizeof(value->subtitle));
    UmiArchiveReadText(reader, value->empty_message, sizeof(value->empty_message));
    value->visible = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelViewArchiveValidate(const UmiPanelView *value)
{
    return umi_panel_view_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_view_archive_encode, umi_panel_view_archive_decode,
    UmiPanelView, UmiPanelViewArchiveSchema, UmiPanelViewArchiveBound, UmiPanelViewArchiveWrite, UmiPanelViewArchiveRead, UmiPanelViewArchiveValidate)
