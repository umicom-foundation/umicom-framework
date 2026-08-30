/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/product/metadata_provider.c
 *
 * PURPOSE:
 *   Implement metadata providers that can feed the reusable Umicom product catalogue.
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
#include "umicom/product/metadata_provider.h"

#include <stdlib.h>
#include <string.h>

struct UmiProductMetadataProviderRegistry {
    UmiProductMetadataProviderSnapshot items[UMI_PRODUCT_METADATA_PROVIDER_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiProductMetadataProviderRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_product_metadata_provider_registry_create(UmiProductMetadataProviderRegistry **out_registry)
{
    UmiProductMetadataProviderRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiProductMetadataProviderRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_product_metadata_provider_registry_destroy(UmiProductMetadataProviderRegistry *registry)
{
    free(registry);
}

UmiStatus umi_product_metadata_provider_registry_upsert(UmiProductMetadataProviderRegistry *registry, const UmiProductMetadataProviderSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_PRODUCT_METADATA_PROVIDER_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiProductMetadataProviderSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_product_metadata_provider_registry_remove(UmiProductMetadataProviderRegistry *registry, const char *id)
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

UmiStatus umi_product_metadata_provider_registry_find(const UmiProductMetadataProviderRegistry *registry, const char *id, UmiProductMetadataProviderSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_product_metadata_provider_registry_at(const UmiProductMetadataProviderRegistry *registry, size_t index, UmiProductMetadataProviderSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_product_metadata_provider_registry_count(const UmiProductMetadataProviderRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_product_metadata_provider_registry_revision(const UmiProductMetadataProviderRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
