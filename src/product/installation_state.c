/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/installation_state.c
 *
 * PURPOSE:
 *   Implement installation, verification and rollback state for reusable product delivery workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/product/installation_state.h"

#include <stdlib.h>
#include <string.h>

struct UmiProductInstallationRegistry {
    UmiProductInstallationSnapshot items[UMI_PRODUCT_INSTALLATION_STATE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiProductInstallationRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_product_installation_state_registry_create(UmiProductInstallationRegistry **out_registry)
{
    UmiProductInstallationRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiProductInstallationRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_product_installation_state_registry_destroy(UmiProductInstallationRegistry *registry)
{
    free(registry);
}

UmiStatus umi_product_installation_state_registry_upsert(UmiProductInstallationRegistry *registry, const UmiProductInstallationSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_PRODUCT_INSTALLATION_STATE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiProductInstallationSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_product_installation_state_registry_remove(UmiProductInstallationRegistry *registry, const char *id)
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

UmiStatus umi_product_installation_state_registry_find(const UmiProductInstallationRegistry *registry, const char *id, UmiProductInstallationSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_product_installation_state_registry_at(const UmiProductInstallationRegistry *registry, size_t index, UmiProductInstallationSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_product_installation_state_registry_set_state(
    UmiProductInstallationRegistry *registry,
    const char *id,
    int state,
    int verified,
    int rollback_available)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    registry->items[index].state = state;
    registry->items[index].verified = verified != 0;
    registry->items[index].rollback_available = rollback_available != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}
size_t umi_product_installation_state_registry_count(const UmiProductInstallationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_product_installation_state_registry_revision(const UmiProductInstallationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
