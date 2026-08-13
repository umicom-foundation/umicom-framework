/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/discovery.c
 *
 * PURPOSE:
 *   Implement a reusable test-explorer and test-run record independent of any single test framework.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/test_platform/discovery.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformDiscoveryRegistry {
    UmiTestPlatformDiscoverySnapshot items[UMI_TEST_PLATFORM_DISCOVERY_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiTestPlatformDiscoveryRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_test_platform_discovery_registry_create(UmiTestPlatformDiscoveryRegistry **out_registry)
{
    UmiTestPlatformDiscoveryRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformDiscoveryRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_test_platform_discovery_registry_destroy(UmiTestPlatformDiscoveryRegistry *registry) { free(registry); }

UmiStatus umi_test_platform_discovery_registry_upsert(UmiTestPlatformDiscoveryRegistry *registry, const UmiTestPlatformDiscoverySnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_TEST_PLATFORM_DISCOVERY_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_DISCOVERY_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].project_id[127U] = '\0';
    registry->items[index].provider[127U] = '\0';
    registry->items[index].root_uri[1023U] = '\0';
    registry->items[index].configuration[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_discovery_registry_remove(UmiTestPlatformDiscoveryRegistry *registry, const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_discovery_registry_find(const UmiTestPlatformDiscoveryRegistry *registry, const char *id, UmiTestPlatformDiscoverySnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_discovery_registry_at(const UmiTestPlatformDiscoveryRegistry *registry, size_t index, UmiTestPlatformDiscoverySnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

size_t umi_test_platform_discovery_registry_count(const UmiTestPlatformDiscoveryRegistry *registry) { return registry != NULL ? registry->count : 0U; }
uint64_t umi_test_platform_discovery_registry_revision(const UmiTestPlatformDiscoveryRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
void umi_test_platform_discovery_registry_clear(UmiTestPlatformDiscoveryRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
