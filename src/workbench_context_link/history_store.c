/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/history_store.c
 *
 * PURPOSE:
 *   Implement bounded dynamic history retention with deterministic back and forward navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/history_store.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextLinkHistoryStore *store)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkHistoryItem *items;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count < store->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = store->capacity == 0U ? 32U : store->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY;
    }
    items = (UmiWorkbenchContextLinkHistoryItem *)realloc(
        store->items, new_capacity * sizeof(store->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (new_capacity > store->capacity) {
        memset(items + store->capacity, 0,
               (new_capacity - store->capacity) * sizeof(items[0]));
    }
    store->items = items;
    store->capacity = new_capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context link history store from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_history_store_init(
    UmiWorkbenchContextLinkHistoryStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

/*
 * Release or reset state held by workbench context link history store so the same storage
 * can be reused safely.
 */
void umi_workbench_context_link_history_store_destroy(
    UmiWorkbenchContextLinkHistoryStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    free(store->items);
    memset(store, 0, sizeof(*store));
}

/*
 * Add workbench context link history store only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_workbench_context_link_history_store_append(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms)
{
    UmiWorkbenchContextLinkHistoryItem *item;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL || payload == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_context_payload_validate(payload) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count == UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY) {
        memmove(&store->items[0], &store->items[1],
                (store->count - 1U) * sizeof(store->items[0]));
        --store->count;
    }
    status = ensure_capacity(store);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    item = &store->items[store->count++];
    memset(item, 0, sizeof(*item));
    status = umi_workbench_context_link_copy_text(
        item->group_id, sizeof(item->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        --store->count;
        return status;
    }
    item->payload = *payload;
    item->sequence = sequence;
    item->published_at_ms = published_at_ms;
    store->cursor = store->count;
    ++store->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench context link history store previous operation used by this module
 * and its client applications.
 */
const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_previous(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL || store->count == 0U) return NULL;
    index = store->cursor > store->count ? store->count : store->cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        --index;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].group_id, group_id) == 0) {
            store->cursor = index;
            return &store->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context link history store next operation used by this module and
 * its client applications.
 */
const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_next(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL || store->count == 0U) return NULL;
    index = store->cursor < store->count ? store->cursor + 1U : store->count;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index < store->count) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].group_id, group_id) == 0) {
            store->cursor = index;
            return &store->items[index];
        }
        ++index;
    }
    return NULL;
}

/*
 * Release or reset state held by workbench context link history store so the same storage
 * can be reused safely.
 */
void umi_workbench_context_link_history_store_clear(
    UmiWorkbenchContextLinkHistoryStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store->items != NULL && store->capacity != 0U) {
        memset(store->items, 0, store->capacity * sizeof(store->items[0]));
    }
    store->count = 0U;
    store->cursor = 0U;
    ++store->revision;
}

/*
 * Return the number of records represented by workbench context link history store without
 * changing their state.
 */
size_t umi_workbench_context_link_history_store_count(
    const UmiWorkbenchContextLinkHistoryStore *store)
{
    return store == NULL ? 0U : store->count;
}
