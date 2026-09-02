/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/progress.c
 *
 * PURPOSE:
 *   Implement an operational workbench service record for problems, output, progress, tasks, notifications, status and navigation.
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
#include "umicom/ui/progress.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiUiProgressRegistry {
    UmiUiProgressSnapshot items[UMI_UI_PROGRESS_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiUiProgressRegistry *registry, const char *id)
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
 * Initialise ui progress registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_progress_registry_create(UmiUiProgressRegistry **out_registry)
{
    UmiUiProgressRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiProgressRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by ui progress registry so the same storage can be reused
 * safely.
 */
void umi_ui_progress_registry_destroy(UmiUiProgressRegistry *registry) { free(registry); }

/*
 * Provide the ui progress registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_progress_registry_upsert(UmiUiProgressRegistry *registry, const UmiUiProgressSnapshot *item)
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
        if (registry->count >= UMI_UI_PROGRESS_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_UI_PROGRESS_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].title[255U] = '\0';
    registry->items[index].detail[511U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove ui progress registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_progress_registry_remove(UmiUiProgressRegistry *registry, const char *id)
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
 * Find ui progress registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_progress_registry_find(const UmiUiProgressRegistry *registry, const char *id, UmiUiProgressSnapshot *out_item)
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
 * Find ui progress registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_progress_registry_at(const UmiUiProgressRegistry *registry, size_t index, UmiUiProgressSnapshot *out_item)
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
 * Return the number of records represented by ui progress registry without changing their
 * state.
 */
size_t umi_ui_progress_registry_count(const UmiUiProgressRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the ui progress registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_progress_registry_revision(const UmiUiProgressRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by ui progress registry so the same storage can be reused
 * safely.
 */
void umi_ui_progress_registry_clear(UmiUiProgressRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}
