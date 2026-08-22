/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/history_store.c
 *
 * PURPOSE:
 *   Implement bounded dynamic history retention with deterministic back and forward navigation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/history_store.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(UmiWorkbenchContextLinkHistoryStore *store)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkHistoryItem *items;
    if (store->count < store->capacity) return UMI_STATUS_OK;
    if (store->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = store->capacity == 0U ? 32U : store->capacity * 2U;
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY;
    }
    items = (UmiWorkbenchContextLinkHistoryItem *)realloc(
        store->items, new_capacity * sizeof(store->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (new_capacity > store->capacity) {
        memset(items + store->capacity, 0,
               (new_capacity - store->capacity) * sizeof(items[0]));
    }
    store->items = items;
    store->capacity = new_capacity;
    return UMI_STATUS_OK;
}

void umi_workbench_context_link_history_store_init(
    UmiWorkbenchContextLinkHistoryStore *store)
{
    if (store == NULL) return;
    memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

void umi_workbench_context_link_history_store_destroy(
    UmiWorkbenchContextLinkHistoryStore *store)
{
    if (store == NULL) return;
    free(store->items);
    memset(store, 0, sizeof(*store));
}

UmiStatus umi_workbench_context_link_history_store_append(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms)
{
    UmiWorkbenchContextLinkHistoryItem *item;
    UmiStatus status;
    if (store == NULL || group_id == NULL || payload == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_context_payload_validate(payload) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (store->count == UMI_WORKBENCH_CONTEXT_LINK_MAX_HISTORY) {
        memmove(&store->items[0], &store->items[1],
                (store->count - 1U) * sizeof(store->items[0]));
        --store->count;
    }
    status = ensure_capacity(store);
    if (status != UMI_STATUS_OK) return status;
    item = &store->items[store->count++];
    memset(item, 0, sizeof(*item));
    status = umi_workbench_context_link_copy_text(
        item->group_id, sizeof(item->group_id), group_id);
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

const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_previous(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id)
{
    size_t index;
    if (store == NULL || group_id == NULL || store->count == 0U) return NULL;
    index = store->cursor > store->count ? store->count : store->cursor;
    while (index > 0U) {
        --index;
        if (strcmp(store->items[index].group_id, group_id) == 0) {
            store->cursor = index;
            return &store->items[index];
        }
    }
    return NULL;
}

const UmiWorkbenchContextLinkHistoryItem *
umi_workbench_context_link_history_store_next(
    UmiWorkbenchContextLinkHistoryStore *store,
    const char *group_id)
{
    size_t index;
    if (store == NULL || group_id == NULL || store->count == 0U) return NULL;
    index = store->cursor < store->count ? store->cursor + 1U : store->count;
    while (index < store->count) {
        if (strcmp(store->items[index].group_id, group_id) == 0) {
            store->cursor = index;
            return &store->items[index];
        }
        ++index;
    }
    return NULL;
}

void umi_workbench_context_link_history_store_clear(
    UmiWorkbenchContextLinkHistoryStore *store)
{
    if (store == NULL) return;
    if (store->items != NULL && store->capacity != 0U) {
        memset(store->items, 0, store->capacity * sizeof(store->items[0]));
    }
    store->count = 0U;
    store->cursor = 0U;
    ++store->revision;
}

size_t umi_workbench_context_link_history_store_count(
    const UmiWorkbenchContextLinkHistoryStore *store)
{
    return store == NULL ? 0U : store->count;
}
