/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/focus_state.c
 *
 * PURPOSE:
 *   Implement cross-application panel focus state validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/focus_state.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel focus state from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_focus_state_init(UmiPanelFocusState *record)
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
/* Check that panel focus state satisfies its contract before another service relies on it. */
UmiStatus umi_panel_focus_state_validate(const UmiPanelFocusState *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->instance_id, '\0', sizeof(record->instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->previous_instance_id, '\0', sizeof(record->previous_instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->previous_instance_id,sizeof(record->previous_instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->instance_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel focus state store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_focus_state_store_init(UmiPanelFocusStateStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel focus state store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelFocusState *umi_panel_focus_state_store_find(UmiPanelFocusStateStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].instance_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel focus state store find const operation used by this module and its
 * client applications.
 */
const UmiPanelFocusState *umi_panel_focus_state_store_find_const(const UmiPanelFocusStateStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].instance_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel focus state store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_focus_state_store_put(UmiPanelFocusStateStore *store,const UmiPanelFocusState *record){
UmiPanelFocusState *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_focus_state_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_focus_state_store_find(store,record->instance_id);
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
 * Remove panel focus state store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_focus_state_store_remove(UmiPanelFocusStateStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].instance_id,identity)==0){
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
 * Provide the panel focus state store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_focus_state_store_snapshot(const UmiPanelFocusStateStore *store,UmiPanelFocusState *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelFocusStateArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa03d86c9de74bb09);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelFocusState *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelFocusState *)0)->previous_instance_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelFocusStateArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelFocusState *)0)->instance_id) - 1U +
        8U + sizeof(((UmiPanelFocusState *)0)->previous_instance_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPanelFocusStateArchiveWrite(UmiArchiveWriter *writer, const UmiPanelFocusState *value)
{
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->previous_instance_id, sizeof(value->previous_instance_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->reason);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->focused);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelFocusStateArchiveRead(UmiArchiveReader *reader, UmiPanelFocusState *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->previous_instance_id, sizeof(value->previous_instance_id));
    value->reason = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->focused = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelFocusStateArchiveValidate(const UmiPanelFocusState *value)
{
    return umi_panel_focus_state_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_focus_state_archive_encode, umi_panel_focus_state_archive_decode,
    UmiPanelFocusState, UmiPanelFocusStateArchiveSchema, UmiPanelFocusStateArchiveBound, UmiPanelFocusStateArchiveWrite, UmiPanelFocusStateArchiveRead, UmiPanelFocusStateArchiveValidate)
