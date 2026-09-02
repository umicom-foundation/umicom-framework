/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/history_store.h
 *
 * PURPOSE:
 *   Retain bounded context history using dynamically allocated storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_STORE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_HISTORY_STORE_H

#include "umicom/workbench_context_link/history_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link history item data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkHistoryItem {
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    UmiContextPayload payload;
    uint64_t sequence;
    uint64_t published_at_ms;
} UmiWorkbenchContextLinkHistoryItem;

/**
 * Represent the workbench context link history store data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkHistoryStore {
    UmiWorkbenchContextLinkHistoryItem *items;
    size_t count;
    size_t cursor;
    size_t capacity;
    uint64_t revision;
} UmiWorkbenchContextLinkHistoryStore;

/**
 * Initialise workbench context link history store from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_history_store_init(
    UmiWorkbenchContextLinkHistoryStore *store);
/**
 * Release or reset state held by workbench context link history store so the same storage
 * can be reused safely.
 */
void umi_workbench_context_link_history_store_destroy(
    UmiWorkbenchContextLinkHistoryStore *store);
/**
 * Add workbench context link history store only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_workbench_context_link_history_store_append(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms);
/**
 * Provide the workbench context link history store previous operation used by this module
 * and its client applications.
 */
const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_previous(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id);
/**
 * Provide the workbench context link history store next operation used by this module and
 * its client applications.
 */
const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_next(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id);
/**
 * Release or reset state held by workbench context link history store so the same storage
 * can be reused safely.
 */
void umi_workbench_context_link_history_store_clear(
    UmiWorkbenchContextLinkHistoryStore *store);
/**
 * Return the number of records represented by workbench context link history store without
 * changing their state.
 */
size_t umi_workbench_context_link_history_store_count(
    const UmiWorkbenchContextLinkHistoryStore *store);

#ifdef __cplusplus
}
#endif

#endif
