/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/extension.c
 *
 * PURPOSE:
 *   Implement chart extension points for indicators, series renderers, overlays and tools.
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
#include "umicom/chart/extension.h"

#include <stdlib.h>
#include <string.h>

struct UmiChartExtensionRegistry {
    UmiChartExtensionSnapshot items[UMI_CHART_EXTENSION_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiChartExtensionRegistry *registry, const char *id)
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
 * Initialise chart extension registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_extension_registry_create(UmiChartExtensionRegistry **out_registry)
{
    UmiChartExtensionRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartExtensionRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by chart extension registry so the same storage can be
 * reused safely.
 */
void umi_chart_extension_registry_destroy(UmiChartExtensionRegistry *registry)
{
    free(registry);
}

/*
 * Provide the chart extension registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_extension_registry_upsert(UmiChartExtensionRegistry *registry, const UmiChartExtensionSnapshot *item)
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
        if (registry->count >= UMI_CHART_EXTENSION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiChartExtensionSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove chart extension registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_extension_registry_remove(UmiChartExtensionRegistry *registry, const char *id)
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
 * Find chart extension registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_extension_registry_find(const UmiChartExtensionRegistry *registry, const char *id, UmiChartExtensionSnapshot *out_item)
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
 * Find chart extension registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_extension_registry_at(const UmiChartExtensionRegistry *registry, size_t index, UmiChartExtensionSnapshot *out_item)
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
 * Return the number of records represented by chart extension registry without changing
 * their state.
 */
size_t umi_chart_extension_registry_count(const UmiChartExtensionRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the chart extension registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_chart_extension_registry_revision(const UmiChartExtensionRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
