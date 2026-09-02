/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/history_entry.h
 *
 * PURPOSE:
 *   Retain bounded delivery history and evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_HISTORY_ENTRY_H
#define UMICOM_CONTEXT_CHANNEL_HISTORY_ENTRY_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_HISTORY_ENTRY_MAX_ITEMS 512U
/**
 * Represent the context history entry data shared with callers of this public contract.
 */
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
/**
 * Represent the context history entry store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextHistoryEntryStore {
    UmiContextHistoryEntry items[UMI_CONTEXT_HISTORY_ENTRY_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextHistoryEntryStore;
/**
 * Initialise context history entry from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_history_entry_init(UmiContextHistoryEntry *record);
/**
 * Check that context history entry satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_history_entry_validate(const UmiContextHistoryEntry *record);
/**
 * Initialise context history entry store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_history_entry_store_init(UmiContextHistoryEntryStore *store);
/**
 * Provide the context history entry store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_history_entry_store_put(UmiContextHistoryEntryStore *store,const UmiContextHistoryEntry *record);
/**
 * Remove context history entry store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_history_entry_store_remove(UmiContextHistoryEntryStore *store,const char *identity);
/**
 * Find context history entry store while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiContextHistoryEntry *umi_context_history_entry_store_find(UmiContextHistoryEntryStore *store,const char *identity);
/**
 * Provide the context history entry store find const operation used by this module and its
 * client applications.
 */
const UmiContextHistoryEntry *umi_context_history_entry_store_find_const(const UmiContextHistoryEntryStore *store,const char *identity);
/**
 * Return the number of records represented by context history entry store without changing
 * their state.
 */
size_t umi_context_history_entry_store_count(const UmiContextHistoryEntryStore *store);
/**
 * Provide the context history entry store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_history_entry_store_snapshot(const UmiContextHistoryEntryStore *store,UmiContextHistoryEntry *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
