/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/resource_location.c
 *
 * PURPOSE:
 *   Implement normalised local and remote resource locations without binding callers to a GUI.
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
#include "umicom/platform/resource_location.h"

#include <stdlib.h>
#include <string.h>

struct UmiResourceLocationRegistry {
    UmiResourceLocationSnapshot items[UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiResourceLocationRegistry *registry, const char *id)
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
 * Initialise platform resource location registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_resource_location_registry_create(UmiResourceLocationRegistry **out_registry)
{
    UmiResourceLocationRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiResourceLocationRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by platform resource location registry so the same storage
 * can be reused safely.
 */
void umi_platform_resource_location_registry_destroy(UmiResourceLocationRegistry *registry)
{
    free(registry);
}

/*
 * Provide the platform resource location registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_platform_resource_location_registry_upsert(UmiResourceLocationRegistry *registry, const UmiResourceLocationSnapshot *item)
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
        if (registry->count >= UMI_PLATFORM_RESOURCE_LOCATION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiResourceLocationSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove platform resource location registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_resource_location_registry_remove(UmiResourceLocationRegistry *registry, const char *id)
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
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_find(const UmiResourceLocationRegistry *registry, const char *id, UmiResourceLocationSnapshot *out_item)
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
 * Find platform resource location registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_platform_resource_location_registry_at(const UmiResourceLocationRegistry *registry, size_t index, UmiResourceLocationSnapshot *out_item)
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
 * Return the number of records represented by platform resource location registry without
 * changing their state.
 */
size_t umi_platform_resource_location_registry_count(const UmiResourceLocationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the platform resource location registry revision operation used by this module
 * and its client applications.
 */
uint64_t umi_platform_resource_location_registry_revision(const UmiResourceLocationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
