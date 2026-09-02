/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/pin_store.c
 *
 * PURPOSE:
 *   Implement compact typed-context pinning with stable identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/pin_store.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextLinkPinStore *store)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkPinItem *items;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count < store->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_PINS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = store->capacity == 0U ? 8U : store->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_PINS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_PINS;
    }
    items = (UmiWorkbenchContextLinkPinItem *)realloc(
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
 * Initialise workbench context link pin store from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_pin_store_init(
    UmiWorkbenchContextLinkPinStore *store)
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
 * Release or reset state held by workbench context link pin store so the same storage can
 * be reused safely.
 */
void umi_workbench_context_link_pin_store_destroy(
    UmiWorkbenchContextLinkPinStore *store)
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
 * Find workbench context link pin store while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchContextLinkPinItem *
umi_workbench_context_link_pin_store_find(
    const UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || pin_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].pin_id, pin_id) == 0) {
            return &store->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context link pin store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_context_link_pin_store_put(
    UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t pinned_at_ms)
{
    size_t index;
    UmiWorkbenchContextLinkPinItem *item = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || pin_id == NULL || group_id == NULL || payload == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_context_payload_validate(payload) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].pin_id, pin_id) == 0) {
            item = &store->items[index];
            break;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) {
        status = ensure_capacity(store);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        item = &store->items[store->count++];
    }
    memset(item, 0, sizeof(*item));
    status = umi_workbench_context_link_copy_text(
        item->pin_id, sizeof(item->pin_id), pin_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_link_copy_text(
        item->group_id, sizeof(item->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    item->payload = *payload;
    item->pinned_at_ms = pinned_at_ms;
    ++store->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench context link pin store while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_context_link_pin_store_remove(
    UmiWorkbenchContextLinkPinStore *store,
    const char *pin_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || pin_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->items[index].pin_id, pin_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < store->count) {
                memmove(&store->items[index], &store->items[index + 1U],
                        (store->count - index - 1U) * sizeof(store->items[0]));
            }
            --store->count;
            memset(&store->items[store->count], 0, sizeof(store->items[0]));
            ++store->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
