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

/* BEGINNER NOTE:
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

static size_t find_index(const UmiResourceLocationRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_platform_resource_location_registry_create(UmiResourceLocationRegistry **out_registry)
{
    UmiResourceLocationRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiResourceLocationRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_platform_resource_location_registry_destroy(UmiResourceLocationRegistry *registry)
{
    free(registry);
}

UmiStatus umi_platform_resource_location_registry_upsert(UmiResourceLocationRegistry *registry, const UmiResourceLocationSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
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

UmiStatus umi_platform_resource_location_registry_remove(UmiResourceLocationRegistry *registry, const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_platform_resource_location_registry_find(const UmiResourceLocationRegistry *registry, const char *id, UmiResourceLocationSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_platform_resource_location_registry_at(const UmiResourceLocationRegistry *registry, size_t index, UmiResourceLocationSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_platform_resource_location_registry_count(const UmiResourceLocationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_platform_resource_location_registry_revision(const UmiResourceLocationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
