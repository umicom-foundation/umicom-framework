/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/history_store.h
 *
 * PURPOSE:
 *   Retain bounded context history using dynamically allocated storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_STORE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_STORE_H

#include "umicom/workbench_context_link/history_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextLinkHistoryItem {
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextPayload payload;
    uint64_t sequence;
    uint64_t published_at_ms;
} UmiWorkbenchContextLinkHistoryItem;

typedef struct UmiWorkbenchContextLinkHistoryStore {
    UmiWorkbenchContextLinkHistoryItem *items;
    size_t count;
    size_t cursor;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkHistoryStore;

void umi_workbench_context_link_history_store_init(
    UmiWorkbenchContextLinkHistoryStore *store);
void umi_workbench_context_link_history_store_destroy(
    UmiWorkbenchContextLinkHistoryStore *store);
UmiStatus umi_workbench_context_link_history_store_append(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms);
const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_previous(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id);
const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_next(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id);
void umi_workbench_context_link_history_store_clear(
    UmiWorkbenchContextLinkHistoryStore *store);
size_t umi_workbench_context_link_history_store_count(
    const UmiWorkbenchContextLinkHistoryStore *store);

#ifdef __cplusplus
}
#endif

#endif
