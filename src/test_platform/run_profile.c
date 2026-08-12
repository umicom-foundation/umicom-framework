/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/run_profile.c
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
#include "umicom/test_platform/run_profile.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformRunProfileRegistry {
    UmiTestPlatformRunProfileSnapshot items[UMI_TEST_PLATFORM_RUN_PROFILE_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiTestPlatformRunProfileRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_test_platform_run_profile_registry_create(UmiTestPlatformRunProfileRegistry **out_registry)
{
    UmiTestPlatformRunProfileRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformRunProfileRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_test_platform_run_profile_registry_destroy(UmiTestPlatformRunProfileRegistry *registry) { free(registry); }

UmiStatus umi_test_platform_run_profile_registry_upsert(UmiTestPlatformRunProfileRegistry *registry, const UmiTestPlatformRunProfileSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_TEST_PLATFORM_RUN_PROFILE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_RUN_PROFILE_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].mode[63U] = '\0';
    registry->items[index].configuration[511U] = '\0';
    registry->items[index].filter[511U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_run_profile_registry_remove(UmiTestPlatformRunProfileRegistry *registry, const char *id)
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

UmiStatus umi_test_platform_run_profile_registry_find(const UmiTestPlatformRunProfileRegistry *registry, const char *id, UmiTestPlatformRunProfileSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

UmiStatus umi_test_platform_run_profile_registry_at(const UmiTestPlatformRunProfileRegistry *registry, size_t index, UmiTestPlatformRunProfileSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

size_t umi_test_platform_run_profile_registry_count(const UmiTestPlatformRunProfileRegistry *registry) { return registry != NULL ? registry->count : 0U; }
uint64_t umi_test_platform_run_profile_registry_revision(const UmiTestPlatformRunProfileRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
void umi_test_platform_run_profile_registry_clear(UmiTestPlatformRunProfileRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
