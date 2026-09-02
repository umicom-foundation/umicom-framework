/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_history.c
 *
 * PURPOSE:
 *   Implement persistent workspace-history records for Studio and future Umicom products.
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
#include "umicom/platform/workspace_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiWorkspaceHistoryRegistry {
    UmiWorkspaceHistorySnapshot items[UMI_PLATFORM_WORKSPACE_HISTORY_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiWorkspaceHistoryRegistry *registry, const char *id)
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
 * Initialise platform workspace history registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_workspace_history_registry_create(UmiWorkspaceHistoryRegistry **out_registry)
{
    UmiWorkspaceHistoryRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiWorkspaceHistoryRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by platform workspace history registry so the same storage
 * can be reused safely.
 */
void umi_platform_workspace_history_registry_destroy(UmiWorkspaceHistoryRegistry *registry)
{
    free(registry);
}

/*
 * Provide the platform workspace history registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_workspace_history_registry_upsert(UmiWorkspaceHistoryRegistry *registry, const UmiWorkspaceHistorySnapshot *item)
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
        if (registry->count >= UMI_PLATFORM_WORKSPACE_HISTORY_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiWorkspaceHistorySnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove platform workspace history registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_workspace_history_registry_remove(UmiWorkspaceHistoryRegistry *registry, const char *id)
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
 * Find platform workspace history registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_workspace_history_registry_find(const UmiWorkspaceHistoryRegistry *registry, const char *id, UmiWorkspaceHistorySnapshot *out_item)
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
 * Find platform workspace history registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_workspace_history_registry_at(const UmiWorkspaceHistoryRegistry *registry, size_t index, UmiWorkspaceHistorySnapshot *out_item)
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
 * Provide the platform workspace history registry touch operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_workspace_history_registry_touch(UmiWorkspaceHistoryRegistry *registry,
                                                           const char *id,
                                                           uint64_t opened_at,
                                                           uint64_t additional_duration_seconds)
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
    registry->items[index].last_opened = opened_at;
    registry->items[index].duration_seconds += additional_duration_seconds;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the platform workspace history registry set trusted operation used by this
 * module and its client applications.
 */
UmiStatus umi_platform_workspace_history_registry_set_trusted(UmiWorkspaceHistoryRegistry *registry,
                                                              const char *id,
                                                              int trusted)
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
    registry->items[index].trusted = trusted != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by platform workspace history registry without
 * changing their state.
 */
size_t umi_platform_workspace_history_registry_count(const UmiWorkspaceHistoryRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the platform workspace history registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_platform_workspace_history_registry_revision(const UmiWorkspaceHistoryRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
