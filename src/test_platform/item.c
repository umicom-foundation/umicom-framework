/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/item.c
 *
 * PURPOSE:
 *   Implement a reusable test-explorer and test-run record independent of any single test framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/test_platform/item.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformItemRegistry {
    UmiTestPlatformItemSnapshot *items;
    size_t capacity;
    size_t *index;
    size_t index_capacity;
    size_t count;
    uint64_t revision;
};

/* Dynamic storage and the identifier index stay private to this registry. */
#include "item_storage.inc"

/*
 * Initialise test platform item registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_item_registry_create(UmiTestPlatformItemRegistry **out_registry)
{
    UmiTestPlatformItemRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformItemRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by test platform item registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_item_registry_destroy(UmiTestPlatformItemRegistry *registry)
{
    if (registry != NULL) {
        free(registry->items);
        free(registry->index);
        free(registry);
    }
}

/*
 * Provide the test platform item registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_item_registry_upsert(UmiTestPlatformItemRegistry *registry, const UmiTestPlatformItemSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = UmiItemValidate(item);
    if (validation != UMI_STATUS_OK) return validation;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_TEST_PLATFORM_ITEM_MAX_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        validation = UmiItemReserve(registry, registry->count + 1U);
        if (validation != UMI_STATUS_OK) return validation;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_ITEM_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].parent_id[127U] = '\0';
    registry->items[index].suite_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].uri[1023U] = '\0';
    registry->items[index].source_uri[1023U] = '\0';
    registry->items[index].framework[127U] = '\0';
    registry->items[index].kind[63U] = '\0';
    registry->items[index].labels[511U] = '\0';
    registry->items[index].command[1023U] = '\0';
    registry->items[index].working_directory[1023U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    UmiItemIndexInsert(registry, index);
    return UMI_STATUS_OK;
}

/*
 * Remove test platform item registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_item_registry_remove(UmiTestPlatformItemRegistry *registry, const char *id)
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
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    UmiItemReindex(registry);
    return UMI_STATUS_OK;
}

/*
 * Find test platform item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_item_registry_find(const UmiTestPlatformItemRegistry *registry, const char *id, UmiTestPlatformItemSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Find test platform item registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_item_registry_at(const UmiTestPlatformItemRegistry *registry, size_t index, UmiTestPlatformItemSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by test platform item registry without changing
 * their state.
 */
size_t umi_test_platform_item_registry_count(const UmiTestPlatformItemRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the test platform item registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_item_registry_revision(const UmiTestPlatformItemRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by test platform item registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_item_registry_clear(UmiTestPlatformItemRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    if (registry->items != NULL) memset(registry->items, 0, registry->count * sizeof(*registry->items));
    registry->count=0U; registry->revision += 1U;
    UmiItemReindex(registry);
}

/* Clone owns its arrays, so aborting a refresh cannot modify a live record. */
UmiStatus UmiTestPlatformItemRegistryClone(const UmiTestPlatformItemRegistry *source,
    UmiTestPlatformItemRegistry **outRegistry)
{
    UmiTestPlatformItemRegistry *copy = NULL;
    UmiStatus status;
    if (outRegistry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outRegistry = NULL;
    if (source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_test_platform_item_registry_create(&copy);
    if (status == UMI_STATUS_OK) status = UmiItemReserve(copy, source->count);
    if (status != UMI_STATUS_OK) {
        umi_test_platform_item_registry_destroy(copy);
        return status;
    }
    if (source->count != 0U)
        (void)memcpy(copy->items, source->items, source->count * sizeof(*copy->items));
    copy->count = source->count;
    copy->revision = source->revision;
    UmiItemReindex(copy);
    *outRegistry = copy;
    return UMI_STATUS_OK;
}

void UmiTestPlatformItemRegistrySwap(UmiTestPlatformItemRegistry *left,
    UmiTestPlatformItemRegistry *right)
{
    UmiTestPlatformItemRegistry temporary;
    if (left == NULL || right == NULL || left == right) return;
    temporary = *left; *left = *right; *right = temporary;
}

UmiStatus UmiTestPlatformItemRegistryRemoveSuite(UmiTestPlatformItemRegistry *registry,
    const char *suiteId)
{
    size_t readIndex, writeIndex = 0U;
    if (registry == NULL || suiteId == NULL || suiteId[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (readIndex = 0U; readIndex < registry->count; ++readIndex) {
        if (strcmp(registry->items[readIndex].suite_id, suiteId) != 0) {
            if (writeIndex != readIndex) registry->items[writeIndex] = registry->items[readIndex];
            ++writeIndex;
        }
    }
    if (writeIndex != registry->count) {
        (void)memset(registry->items + writeIndex, 0,
            (registry->count - writeIndex) * sizeof(*registry->items));
        registry->count = writeIndex;
        ++registry->revision;
        UmiItemReindex(registry);
    }
    return UMI_STATUS_OK;
}
