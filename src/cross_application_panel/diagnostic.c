/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/diagnostic.c
 *
 * PURPOSE:
 *   Implement cross-application panel diagnostic validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/diagnostic.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise panel diagnostic from caller-provided values so later operations receive a
 * known state.
 */
void umi_panel_diagnostic_init(UmiPanelDiagnostic *record)
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
/* Check that panel diagnostic satisfies its contract before another service relies on it. */
UmiStatus umi_panel_diagnostic_validate(const UmiPanelDiagnostic *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->diagnostic_id, '\0', sizeof(record->diagnostic_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->panel_id, '\0', sizeof(record->panel_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->instance_id, '\0', sizeof(record->instance_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->message, '\0', sizeof(record->message)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->diagnostic_id,sizeof(record->diagnostic_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(!umi_context_text_is_valid(record->message,sizeof(record->message)))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->diagnostic_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
/*
 * Initialise panel diagnostic store from caller-provided values so later operations
 * receive a known state.
 */
void umi_panel_diagnostic_store_init(UmiPanelDiagnosticStore *store){
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find panel diagnostic store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiPanelDiagnostic *umi_panel_diagnostic_store_find(UmiPanelDiagnosticStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].diagnostic_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel diagnostic store find const operation used by this module and its
 * client applications.
 */
const UmiPanelDiagnostic *umi_panel_diagnostic_store_find_const(const UmiPanelDiagnosticStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].diagnostic_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the panel diagnostic store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostic_store_put(UmiPanelDiagnosticStore *store,const UmiPanelDiagnostic *record){
UmiPanelDiagnostic *existing;
uint64_t next;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_panel_diagnostic_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_diagnostic_store_find(store,record->diagnostic_id);
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
 * Remove panel diagnostic store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_panel_diagnostic_store_remove(UmiPanelDiagnosticStore *store,const char *identity){
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].diagnostic_id,identity)==0){
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
 * Provide the panel diagnostic store snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_panel_diagnostic_store_snapshot(const UmiPanelDiagnosticStore *store,UmiPanelDiagnostic *records,size_t capacity,size_t *out_count){
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
static uint64_t UmiPanelDiagnosticArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7e2bb0f2b0824701);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDiagnostic *)0)->diagnostic_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDiagnostic *)0)->panel_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDiagnostic *)0)->instance_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiPanelDiagnostic *)0)->message)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiPanelDiagnosticArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiPanelDiagnostic *)0)->diagnostic_id) - 1U +
        8U + sizeof(((UmiPanelDiagnostic *)0)->panel_id) - 1U +
        8U + sizeof(((UmiPanelDiagnostic *)0)->instance_id) - 1U +
        8U + sizeof(((UmiPanelDiagnostic *)0)->message) - 1U +
        8U +
        8U +
        8U;
}
static void UmiPanelDiagnosticArchiveWrite(UmiArchiveWriter *writer, const UmiPanelDiagnostic *value)
{
    UmiArchiveWriteText(writer, value->diagnostic_id, sizeof(value->diagnostic_id));
    UmiArchiveWriteText(writer, value->panel_id, sizeof(value->panel_id));
    UmiArchiveWriteText(writer, value->instance_id, sizeof(value->instance_id));
    UmiArchiveWriteText(writer, value->message, sizeof(value->message));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->severity);
    UmiArchiveWriteSigned(writer, (int64_t)value->status);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiPanelDiagnosticArchiveRead(UmiArchiveReader *reader, UmiPanelDiagnostic *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->diagnostic_id, sizeof(value->diagnostic_id));
    UmiArchiveReadText(reader, value->panel_id, sizeof(value->panel_id));
    UmiArchiveReadText(reader, value->instance_id, sizeof(value->instance_id));
    UmiArchiveReadText(reader, value->message, sizeof(value->message));
    value->severity = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->status = (UmiStatus)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiPanelDiagnosticArchiveValidate(const UmiPanelDiagnostic *value)
{
    return umi_panel_diagnostic_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_panel_diagnostic_archive_encode, umi_panel_diagnostic_archive_decode,
    UmiPanelDiagnostic, UmiPanelDiagnosticArchiveSchema, UmiPanelDiagnosticArchiveBound, UmiPanelDiagnosticArchiveWrite, UmiPanelDiagnosticArchiveRead, UmiPanelDiagnosticArchiveValidate)
