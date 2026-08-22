/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/active_context_store.c
 *
 * PURPOSE:
 *   Implement compact per-group active context retention without toolkit-owned state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/active_context_store.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(UmiWorkbenchContextLinkActiveContextStore *store)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkActiveSlot *slots;
    if (store->count < store->capacity) return UMI_STATUS_OK;
    if (store->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = store->capacity == 0U ? 4U : store->capacity * 2U;
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS;
    }
    slots = (UmiWorkbenchContextLinkActiveSlot *)realloc(
        store->slots, new_capacity * sizeof(store->slots[0]));
    if (slots == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (new_capacity > store->capacity) {
        memset(slots + store->capacity, 0,
               (new_capacity - store->capacity) * sizeof(slots[0]));
    }
    store->slots = slots;
    store->capacity = new_capacity;
    return UMI_STATUS_OK;
}

void umi_workbench_context_link_active_context_store_init(
    UmiWorkbenchContextLinkActiveContextStore *store)
{
    if (store == NULL) return;
    memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

void umi_workbench_context_link_active_context_store_destroy(
    UmiWorkbenchContextLinkActiveContextStore *store)
{
    if (store == NULL) return;
    free(store->slots);
    memset(store, 0, sizeof(*store));
}

const UmiWorkbenchContextLinkActiveSlot *
umi_workbench_context_link_active_context_store_find(
    const UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id)
{
    size_t index;
    if (store == NULL || group_id == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->slots[index].group_id, group_id) == 0) {
            return &store->slots[index];
        }
    }
    return NULL;
}

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
    if (store == NULL || group_id == NULL || payload == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_context_payload_validate(payload) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->slots[index].group_id, group_id) == 0) {
            slot = &store->slots[index];
            break;
        }
    }
    if (slot == NULL) {
        status = ensure_capacity(store);
        if (status != UMI_STATUS_OK) return status;
        slot = &store->slots[store->count++];
    }
    memset(slot, 0, sizeof(*slot));
    status = umi_workbench_context_link_copy_text(
        slot->group_id, sizeof(slot->group_id), group_id);
    if (status != UMI_STATUS_OK) return status;
    slot->payload = *payload;
    slot->sequence = sequence;
    slot->published_at_ms = published_at_ms;
    slot->revision = store->revision + 1U;
    slot->occupied = true;
    ++store->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_link_active_context_store_clear(
    UmiWorkbenchContextLinkActiveContextStore *store,
    const char *group_id)
{
    size_t index;
    if (store == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) {
        if (strcmp(store->slots[index].group_id, group_id) == 0) {
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
