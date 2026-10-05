/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/definition.c
 *
 * PURPOSE:
 *   Implement cross-application panel definition validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/definition.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel definition from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_definition_init(UmiPanelDefinition *record)
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
/* Check that panel definition satisfies its contract before another service relies on it. */
UmiStatus umi_panel_definition_validate(const UmiPanelDefinition *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->application_id, '\0', sizeof(record->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->title, '\0', sizeof(record->title)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->description, '\0', sizeof(record->description)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->component_id, '\0', sizeof(record->component_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->default_channel_id, '\0', sizeof(record->default_channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->category, '\0', sizeof(record->category)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->application_id,sizeof(record->application_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->title,sizeof(record->title)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->description,sizeof(record->description)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->component_id,sizeof(record->component_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->default_channel_id,sizeof(record->default_channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->category,sizeof(record->category)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->panel_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel definition store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_definition_store_init(UmiPanelDefinitionStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel definition store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelDefinition *umi_panel_definition_store_find(UmiPanelDefinitionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].panel_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel definition store find const operation used by this module and its
 * client applications.
 */
const UmiPanelDefinition *umi_panel_definition_store_find_const(const UmiPanelDefinitionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].panel_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel definition store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_definition_store_put(UmiPanelDefinitionStore *store,const UmiPanelDefinition *record){
UmiPanelDefinition *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_definition_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_definition_store_find(store,record->panel_id);
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
 * Remove panel definition store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_definition_store_remove(UmiPanelDefinitionStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].panel_id,identity)==0){
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
 * Provide the panel definition store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_definition_store_snapshot(const UmiPanelDefinitionStore *store,UmiPanelDefinition *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelDefinitionArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xfea892c4c89251f0);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->title)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->description)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->default_channel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDefinition *)0)->category)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelDefinitionArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelDefinition *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelDefinition *)0)->application_id) - 1U +
        8U + sizeof(((UmiPanelDefinition *)0)->title) - 1U +
        8U + sizeof(((UmiPanelDefinition *)0)->description) - 1U +
        8U + sizeof(((UmiPanelDefinition *)0)->component_id) - 1U +
        8U + sizeof(((UmiPanelDefinition *)0)->default_channel_id) - 1U +
        8U + sizeof(((UmiPanelDefinition *)0)->category) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiPanelDefinitionArchiveWrite(UmiArchiveWriter *writer, const UmiPanelDefinition *value)
{
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteText(writer, value->description, sizeof(value->description));
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->default_channel_id, sizeof(value->default_channel_id));
    UmiArchiveWriteText(writer, value->category, sizeof(value->category));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->singleton);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->context_aware);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelDefinitionArchiveRead(UmiArchiveReader *reader, UmiPanelDefinition *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    UmiArchiveReadText(reader, value->description, sizeof(value->description));
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->default_channel_id, sizeof(value->default_channel_id));
    UmiArchiveReadText(reader, value->category, sizeof(value->category));
    value->singleton = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->context_aware = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelDefinitionArchiveValidate(const UmiPanelDefinition *value)
{
    return umi_panel_definition_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_definition_archive_encode, umi_panel_definition_archive_decode,
    UmiPanelDefinition, UmiPanelDefinitionArchiveSchema, UmiPanelDefinitionArchiveBound, UmiPanelDefinitionArchiveWrite, UmiPanelDefinitionArchiveRead, UmiPanelDefinitionArchiveValidate)
