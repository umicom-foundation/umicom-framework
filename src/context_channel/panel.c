/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/panel.c
 *
 * PURPOSE:
 *   Implement register reusable cross-application panel contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/panel.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context panel from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_panel_init(UmiCrossApplicationPanel *record)
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
/* Check that context panel satisfies its contract before another service relies on it. */
UmiStatus umi_context_panel_validate(const UmiCrossApplicationPanel *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->application_id, '\0', sizeof(record->application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->title, '\0', sizeof(record->title)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->component_id, '\0', sizeof(record->component_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->default_channel_id, '\0', sizeof(record->default_channel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->panel_id, sizeof(record->panel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->application_id, sizeof(record->application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->title, sizeof(record->title))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->component_id, sizeof(record->component_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->default_channel_id, sizeof(record->default_channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->panel_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context panel store from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_panel_store_init(UmiCrossApplicationPanelStore *store)
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
 * Find context panel store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiCrossApplicationPanel *umi_context_panel_store_find(UmiCrossApplicationPanelStore *store,const char *identity)
{
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
 * Provide the context panel store find const operation used by this module and its client
 * applications.
 */
const UmiCrossApplicationPanel *umi_context_panel_store_find_const(const UmiCrossApplicationPanelStore *store,const char *identity)
{
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
 * Provide the context panel store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_panel_store_put(UmiCrossApplicationPanelStore *store,const UmiCrossApplicationPanel *record)
{
    UmiCrossApplicationPanel *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_panel_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_panel_store_find(store,record->panel_id);
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
    if(store->count>=UMI_CONTEXT_PANEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context panel store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_panel_store_remove(UmiCrossApplicationPanelStore *store,const char *identity)
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
if(strcmp(store->items[i].panel_id,identity)==0){
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
 * Return the number of records represented by context panel store without changing their
 * state.
 */
size_t umi_context_panel_store_count(const UmiCrossApplicationPanelStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context panel store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_panel_store_snapshot(const UmiCrossApplicationPanelStore *store,UmiCrossApplicationPanel *out_records,size_t capacity,size_t *out_count)
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
static uint64_t UmiCrossApplicationPanelArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x38063a268dd99907);
    schema = (schema ^ (uint64_t)sizeof(((UmiCrossApplicationPanel *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCrossApplicationPanel *)0)->application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCrossApplicationPanel *)0)->title)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCrossApplicationPanel *)0)->component_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCrossApplicationPanel *)0)->default_channel_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCrossApplicationPanelArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCrossApplicationPanel *)0)->panel_id) - 1U +
        8U + sizeof(((UmiCrossApplicationPanel *)0)->application_id) - 1U +
        8U + sizeof(((UmiCrossApplicationPanel *)0)->title) - 1U +
        8U + sizeof(((UmiCrossApplicationPanel *)0)->component_id) - 1U +
        8U + sizeof(((UmiCrossApplicationPanel *)0)->default_channel_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiCrossApplicationPanelArchiveWrite(UmiArchiveWriter *writer, const UmiCrossApplicationPanel *value)
{
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->application_id, sizeof(value->application_id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteText(writer, value->component_id, sizeof(value->component_id));
    UmiArchiveWriteText(writer, value->default_channel_id, sizeof(value->default_channel_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->singleton);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->context_aware);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiCrossApplicationPanelArchiveRead(UmiArchiveReader *reader, UmiCrossApplicationPanel *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->application_id, sizeof(value->application_id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    UmiArchiveReadText(reader, value->component_id, sizeof(value->component_id));
    UmiArchiveReadText(reader, value->default_channel_id, sizeof(value->default_channel_id));
    value->singleton = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->context_aware = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiCrossApplicationPanelArchiveValidate(const UmiCrossApplicationPanel *value)
{
    return umi_context_panel_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_panel_archive_encode, umi_context_panel_archive_decode,
    UmiCrossApplicationPanel, UmiCrossApplicationPanelArchiveSchema, UmiCrossApplicationPanelArchiveBound, UmiCrossApplicationPanelArchiveWrite, UmiCrossApplicationPanelArchiveRead, UmiCrossApplicationPanelArchiveValidate)
