/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/result.c
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
#include "umicom/test_platform/result.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformResultRegistry {
    UmiTestPlatformResultSnapshot items[UMI_TEST_PLATFORM_RESULT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiTestPlatformResultRegistry *registry, const char *id)
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
 * Initialise test platform result registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_test_platform_result_registry_create(UmiTestPlatformResultRegistry **out_registry)
{
    UmiTestPlatformResultRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformResultRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by test platform result registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_result_registry_destroy(UmiTestPlatformResultRegistry *registry) { free(registry); }

/*
 * Provide the test platform result registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_platform_result_registry_upsert(UmiTestPlatformResultRegistry *registry, const UmiTestPlatformResultSnapshot *item)
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
        if (registry->count >= UMI_TEST_PLATFORM_RESULT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_RESULT_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].session_id[127U] = '\0';
    registry->items[index].item_id[127U] = '\0';
    registry->items[index].message[1023U] = '\0';
    registry->items[index].failure_details[2047U] = '\0';
    registry->items[index].attachment_id[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the test platform outcome text operation used by this module and its client
 * applications.
 */
const char *umi_test_platform_outcome_text(UmiTestPlatformOutcome outcome)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (outcome) {
        case UMI_TEST_PLATFORM_OUTCOME_NOT_RUN: return "not-run";
        case UMI_TEST_PLATFORM_OUTCOME_PASSED: return "passed";
        case UMI_TEST_PLATFORM_OUTCOME_FAILED: return "failed";
        case UMI_TEST_PLATFORM_OUTCOME_SKIPPED: return "skipped";
        case UMI_TEST_PLATFORM_OUTCOME_CANCELLED: return "cancelled";
        case UMI_TEST_PLATFORM_OUTCOME_TIMED_OUT: return "timed-out";
        default: return "unknown";
    }
}

/*
 * Remove test platform result registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_test_platform_result_registry_remove(UmiTestPlatformResultRegistry *registry, const char *id)
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
    return UMI_STATUS_OK;
}

/*
 * Find test platform result registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_result_registry_find(const UmiTestPlatformResultRegistry *registry, const char *id, UmiTestPlatformResultSnapshot *out_item)
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
 * Find test platform result registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_test_platform_result_registry_at(const UmiTestPlatformResultRegistry *registry, size_t index, UmiTestPlatformResultSnapshot *out_item)
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
 * Return the number of records represented by test platform result registry without
 * changing their state.
 */
size_t umi_test_platform_result_registry_count(const UmiTestPlatformResultRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the test platform result registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_test_platform_result_registry_revision(const UmiTestPlatformResultRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by test platform result registry so the same storage can be
 * reused safely.
 */
void umi_test_platform_result_registry_clear(UmiTestPlatformResultRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
