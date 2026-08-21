/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/history_entry.h
 *
 * PURPOSE:
 *   Retain bounded delivery history and evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_HISTORY_ENTRY_H
#define UMICOM_CONTEXT_CHANNEL_HISTORY_ENTRY_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_HISTORY_ENTRY_MAX_ITEMS 512U
typedef struct UmiContextHistoryEntry {
    uint32_t structure_size;
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    char channel_id[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_panel_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t sequence;
    uint64_t content_hash;
    UmiContextDeliveryState state;
    uint64_t revision;
} UmiContextHistoryEntry;
typedef struct UmiContextHistoryEntryStore {
    UmiContextHistoryEntry items[UMI_CONTEXT_HISTORY_ENTRY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextHistoryEntryStore;
void umi_context_history_entry_init(UmiContextHistoryEntry *record);
UmiStatus umi_context_history_entry_validate(const UmiContextHistoryEntry *record);
void umi_context_history_entry_store_init(UmiContextHistoryEntryStore *store);
UmiStatus umi_context_history_entry_store_put(UmiContextHistoryEntryStore *store,const UmiContextHistoryEntry *record);
UmiStatus umi_context_history_entry_store_remove(UmiContextHistoryEntryStore *store,const char *identity);
UmiContextHistoryEntry *umi_context_history_entry_store_find(UmiContextHistoryEntryStore *store,const char *identity);
const UmiContextHistoryEntry *umi_context_history_entry_store_find_const(const UmiContextHistoryEntryStore *store,const char *identity);
size_t umi_context_history_entry_store_count(const UmiContextHistoryEntryStore *store);
UmiStatus umi_context_history_entry_store_snapshot(const UmiContextHistoryEntryStore *store,UmiContextHistoryEntry *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
