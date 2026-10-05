/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/command.c
 *
 * PURPOSE:
 *   Implement cross-application panel command validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/command.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel command from caller-provided values so later operations receive a known
 * state.
 */
void umi_panel_command_init(UmiPanelCommand *record)
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
/* Check that panel command satisfies its contract before another service relies on it. */
UmiStatus umi_panel_command_validate(const UmiPanelCommand *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->command_id, '\0', sizeof(record->command_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->instance_id, '\0', sizeof(record->instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->action_id, '\0', sizeof(record->action_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->argument, '\0', sizeof(record->argument)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->command_id,sizeof(record->command_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->action_id,sizeof(record->action_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->argument,sizeof(record->argument)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->command_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel command store from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_command_store_init(UmiPanelCommandStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel command store while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiPanelCommand *umi_panel_command_store_find(UmiPanelCommandStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].command_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel command store find const operation used by this module and its client
 * applications.
 */
const UmiPanelCommand *umi_panel_command_store_find_const(const UmiPanelCommandStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].command_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel command store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_command_store_put(UmiPanelCommandStore *store,const UmiPanelCommand *record){
UmiPanelCommand *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_command_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_command_store_find(store,record->command_id);
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
 * Remove panel command store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_command_store_remove(UmiPanelCommandStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].command_id,identity)==0){
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
 * Provide the panel command store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_command_store_snapshot(const UmiPanelCommandStore *store,UmiPanelCommand *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelCommandArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb80fc048fea8709b);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelCommand *)0)->command_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelCommand *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelCommand *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelCommand *)0)->action_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelCommand *)0)->argument)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelCommandArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelCommand *)0)->command_id) - 1U +
        8U + sizeof(((UmiPanelCommand *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelCommand *)0)->instance_id) - 1U +
        8U + sizeof(((UmiPanelCommand *)0)->action_id) - 1U +
        8U + sizeof(((UmiPanelCommand *)0)->argument) - 1U +
        8U +
        8U;
}
static void UmiPanelCommandArchiveWrite(UmiArchiveWriter *writer, const UmiPanelCommand *value)
{
    UmiArchiveWriteText(writer, value->command_id, sizeof(value->command_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->action_id, sizeof(value->action_id));
    UmiArchiveWriteText(writer, value->argument, sizeof(value->argument));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->timestamp_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelCommandArchiveRead(UmiArchiveReader *reader, UmiPanelCommand *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->command_id, sizeof(value->command_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->action_id, sizeof(value->action_id));
    UmiArchiveReadText(reader, value->argument, sizeof(value->argument));
    value->timestamp_ms = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelCommandArchiveValidate(const UmiPanelCommand *value)
{
    return umi_panel_command_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_command_archive_encode, umi_panel_command_archive_decode,
    UmiPanelCommand, UmiPanelCommandArchiveSchema, UmiPanelCommandArchiveBound, UmiPanelCommandArchiveWrite, UmiPanelCommandArchiveRead, UmiPanelCommandArchiveValidate)
