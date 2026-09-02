/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/signal.c
 *
 * PURPOSE:
 *   Implement signal-to-command bindings for server-side and desktop frontend composition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/frontend/signal.h"

#include <stdlib.h>
#include <string.h>

struct UmiFrontendSignalRegistry {
    UmiFrontendSignalSnapshot items[UMI_FRONTEND_SIGNAL_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendSignalRegistry *registry, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

/*
 * Initialise frontend signal registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_signal_registry_create(UmiFrontendSignalRegistry **out_registry)
{
    UmiFrontendSignalRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiFrontendSignalRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by frontend signal registry so the same storage can be
 * reused safely.
 */
void umi_frontend_signal_registry_destroy(UmiFrontendSignalRegistry *registry)
{
    free(registry);
}

/*
 * Provide the frontend signal registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_frontend_signal_registry_upsert(UmiFrontendSignalRegistry *registry, const UmiFrontendSignalSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_FRONTEND_SIGNAL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiFrontendSignalSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove frontend signal registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_signal_registry_remove(UmiFrontendSignalRegistry *registry, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find frontend signal registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_signal_registry_find(const UmiFrontendSignalRegistry *registry, const char *id, UmiFrontendSignalSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find frontend signal registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_frontend_signal_registry_at(const UmiFrontendSignalRegistry *registry, size_t index, UmiFrontendSignalSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by frontend signal registry without changing
 * their state.
 */
size_t umi_frontend_signal_registry_count(const UmiFrontendSignalRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the frontend signal registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_signal_registry_revision(const UmiFrontendSignalRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
