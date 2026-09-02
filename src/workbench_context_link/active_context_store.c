/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/active_context_store.c
 *
 * PURPOSE:
 *   Implement compact per-group active context retention without toolkit-owned state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/active_context_store.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextLinkActiveContextStore *store)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkActiveSlot *slots;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count < store->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = store->capacity == 0U ? 4U : store->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS;
    }
    slots = (UmiWorkbenchContextLinkActiveSlot *)realloc(
        store->slots, new_capacity * sizeof(store->slots[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slots == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (new_capacity > store->capacity) {
        memset(slots + store->capacity, 0,
               (new_capacity - store->capacity) * sizeof(slots[0]));
    }
    store->slots = slots;
    store->capacity = new_capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context link active context store from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_link_active_context_store_init(
    UmiWorkbenchContextLinkActiveContextStore *store)
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
 * Release or reset state held by workbench context link active context store so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_active_context_store_destroy(
    UmiWorkbenchContextLinkActiveContextStore *store)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL) return;
    free(store->slots);
    memset(store, 0, sizeof(*store));
}

/*
 * Find workbench context link active context store while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_active_context_store_find(
    const UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->slots[index].group_id, group_id) == 0) {
            return &store->slots[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context link active context store put operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_active_context_store_put(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id,
    const UmiContextPayload *payload,
    uint64_t sequence,
    uint64_t published_at_ms)
{
    size_t index;
    UmiWorkbenchContextLinkActiveSlot *slot = NULL;
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->slots[index].group_id, group_id) == 0) {
            slot = &store->slots[index];
            break;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slot == NULL) {
        status = ensure_capacity(store);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        slot = &store->slots[store->count++];
    }
    memset(slot, 0, sizeof(*slot));
    status = umi_workbench_context_link_copy_text(
        slot->group_id, sizeof(slot->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    slot->payload = *payload;
    slot->sequence = sequence;
    slot->published_at_ms = published_at_ms;
    slot->revision = store->revision + 1U;
    slot->occupied = true;
    ++store->revision;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by workbench context link active context store so the same
 * storage can be reused safely.
 */
UmiStatus umi_workbench_context_link_active_context_store_clear(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(store->slots[index].group_id, group_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < store->count) {
                memmove(&store->slots[index], &store->slots[index + 1U],
                        (store->count - index - 1U) * sizeof(store->slots[0]));
            }
            --store->count;
            memset(&store->slots[store->count], 0, sizeof(store->slots[0]));
            ++store->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
