/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/file_operation_queue.c
 *
 * PURPOSE:
 *   Implement deterministic queued file operations suitable for file-manager and IDE workflows.
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
#include "umicom/platform/file_operation_queue.h"

#include <stdlib.h>
#include <string.h>

struct UmiFileOperationRegistry {
    UmiFileOperationSnapshot items[UMI_PLATFORM_FILE_OPERATION_QUEUE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFileOperationRegistry *registry, const char *id)
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
 * Initialise platform file operation queue registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_platform_file_operation_queue_registry_create(UmiFileOperationRegistry **out_registry)
{
    UmiFileOperationRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiFileOperationRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by platform file operation queue registry so the same
 * storage can be reused safely.
 */
void umi_platform_file_operation_queue_registry_destroy(UmiFileOperationRegistry *registry)
{
    free(registry);
}

/*
 * Provide the platform file operation queue registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_platform_file_operation_queue_registry_upsert(UmiFileOperationRegistry *registry, const UmiFileOperationSnapshot *item)
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
        if (registry->count >= UMI_PLATFORM_FILE_OPERATION_QUEUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiFileOperationSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove platform file operation queue registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_platform_file_operation_queue_registry_remove(UmiFileOperationRegistry *registry, const char *id)
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
 * Find platform file operation queue registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_platform_file_operation_queue_registry_find(const UmiFileOperationRegistry *registry, const char *id, UmiFileOperationSnapshot *out_item)
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
 * Find platform file operation queue registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_platform_file_operation_queue_registry_at(const UmiFileOperationRegistry *registry, size_t index, UmiFileOperationSnapshot *out_item)
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
 * Provide the platform file operation queue registry update progress operation used by
 * this module and its client applications.
 */
UmiStatus umi_platform_file_operation_queue_registry_update_progress(
    UmiFileOperationRegistry *registry,
    const char *id,
    uint64_t bytes_done,
    int state,
    const char *error_text)
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
    if (registry->items[index].bytes_total != 0U && bytes_done > registry->items[index].bytes_total) {
        bytes_done = registry->items[index].bytes_total;
    }
    registry->items[index].bytes_done = bytes_done;
    registry->items[index].state = state;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (error_text != NULL) {
        size_t length = strlen(error_text);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(registry->items[index].error_text)) length = sizeof(registry->items[index].error_text) - 1U;
        memcpy(registry->items[index].error_text, error_text, length);
        registry->items[index].error_text[length] = '\0';
    }
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by platform file operation queue registry
 * without changing their state.
 */
size_t umi_platform_file_operation_queue_registry_count(const UmiFileOperationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the platform file operation queue registry revision operation used by this
 * module and its client applications.
 */
uint64_t umi_platform_file_operation_queue_registry_revision(const UmiFileOperationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
