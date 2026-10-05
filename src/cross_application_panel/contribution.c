/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/contribution.c
 *
 * PURPOSE:
 *   Implement cross-application panel contribution validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/contribution.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel contribution from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_contribution_init(UmiPanelContribution *record)
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
 * Check that panel contribution satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_panel_contribution_validate(const UmiPanelContribution *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->contribution_id, '\0', sizeof(record->contribution_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->application_id, '\0', sizeof(record->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->menu_path, '\0', sizeof(record->menu_path)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->command_id, '\0', sizeof(record->command_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->icon_resource_id, '\0', sizeof(record->icon_resource_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->contribution_id,sizeof(record->contribution_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->application_id,sizeof(record->application_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->menu_path,sizeof(record->menu_path)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->command_id,sizeof(record->command_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->icon_resource_id,sizeof(record->icon_resource_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->contribution_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel contribution store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_contribution_store_init(UmiPanelContributionStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel contribution store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelContribution *umi_panel_contribution_store_find(UmiPanelContributionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].contribution_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel contribution store find const operation used by this module and its
 * client applications.
 */
const UmiPanelContribution *umi_panel_contribution_store_find_const(const UmiPanelContributionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].contribution_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel contribution store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_contribution_store_put(UmiPanelContributionStore *store,const UmiPanelContribution *record){
UmiPanelContribution *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_contribution_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_contribution_store_find(store,record->contribution_id);
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
 * Remove panel contribution store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_contribution_store_remove(UmiPanelContributionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].contribution_id,identity)==0){
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
 * Provide the panel contribution store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_panel_contribution_store_snapshot(const UmiPanelContributionStore *store,UmiPanelContribution *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelContributionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7582304acdafb4d5);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelContribution *)0)->contribution_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelContribution *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelContribution *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelContribution *)0)->menu_path)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelContribution *)0)->command_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelContribution *)0)->icon_resource_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelContributionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelContribution *)0)->contribution_id) - 1U +
        8U + sizeof(((UmiPanelContribution *)0)->application_id) - 1U +
        8U + sizeof(((UmiPanelContribution *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelContribution *)0)->menu_path) - 1U +
        8U + sizeof(((UmiPanelContribution *)0)->command_id) - 1U +
        8U + sizeof(((UmiPanelContribution *)0)->icon_resource_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiPanelContributionArchiveWrite(UmiArchiveWriter *writer, const UmiPanelContribution *value)
{
    UmiArchiveWriteText(writer, value->contribution_id, sizeof(value->contribution_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->menu_path, sizeof(value->menu_path));
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteText(writer, value->icon_resource_id, sizeof(value->icon_resource_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelContributionArchiveRead(UmiArchiveReader *reader, UmiPanelContribution *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->contribution_id, sizeof(value->contribution_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->menu_path, sizeof(value->menu_path));
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    UmiArchiveReadText(reader, value->icon_resource_id, sizeof(value->icon_resource_id));
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelContributionArchiveValidate(const UmiPanelContribution *value)
{
    return umi_panel_contribution_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_contribution_archive_encode, umi_panel_contribution_archive_decode,
    UmiPanelContribution, UmiPanelContributionArchiveSchema, UmiPanelContributionArchiveBound, UmiPanelContributionArchiveWrite, UmiPanelContributionArchiveRead, UmiPanelContributionArchiveValidate)
