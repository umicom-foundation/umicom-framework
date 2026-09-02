/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/attachment.c
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
#include "umicom/test_platform/attachment.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiTestPlatformAttachmentRegistry {
    UmiTestPlatformAttachmentSnapshot items[UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiTestPlatformAttachmentRegistry *registry, const char *id)
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
 * Initialise test platform attachment registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_test_platform_attachment_registry_create(UmiTestPlatformAttachmentRegistry **out_registry)
{
    UmiTestPlatformAttachmentRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiTestPlatformAttachmentRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by test platform attachment registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_attachment_registry_destroy(UmiTestPlatformAttachmentRegistry *registry) { free(registry); }

/*
 * Provide the test platform attachment registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_platform_attachment_registry_upsert(UmiTestPlatformAttachmentRegistry *registry, const UmiTestPlatformAttachmentSnapshot *item)
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
        if (registry->count >= UMI_TEST_PLATFORM_ATTACHMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_TEST_PLATFORM_ATTACHMENT_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].result_id[127U] = '\0';
    registry->items[index].name[255U] = '\0';
    registry->items[index].kind[63U] = '\0';
    registry->items[index].producer[127U] = '\0';
    registry->items[index].uri[1023U] = '\0';
    registry->items[index].mime_type[127U] = '\0';
    registry->items[index].schema_uri[1023U] = '\0';
    registry->items[index].checksum[127U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove test platform attachment registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_test_platform_attachment_registry_remove(UmiTestPlatformAttachmentRegistry *registry, const char *id)
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
 * Find test platform attachment registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_attachment_registry_find(const UmiTestPlatformAttachmentRegistry *registry, const char *id, UmiTestPlatformAttachmentSnapshot *out_item)
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
 * Find test platform attachment registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_test_platform_attachment_registry_at(const UmiTestPlatformAttachmentRegistry *registry, size_t index, UmiTestPlatformAttachmentSnapshot *out_item)
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
 * Return the number of records represented by test platform attachment registry without
 * changing their state.
 */
size_t umi_test_platform_attachment_registry_count(const UmiTestPlatformAttachmentRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the test platform attachment registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_test_platform_attachment_registry_revision(const UmiTestPlatformAttachmentRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by test platform attachment registry so the same storage can
 * be reused safely.
 */
void umi_test_platform_attachment_registry_clear(UmiTestPlatformAttachmentRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
