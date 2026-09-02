/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/crosshair.c
 *
 * PURPOSE:
 *   Implement synchronised crosshair state shared across chart panes and series.
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
#include "umicom/chart/crosshair.h"

#include <stdlib.h>
#include <string.h>

struct UmiChartCrosshairRegistry {
    UmiChartCrosshairSnapshot items[UMI_CHART_CROSSHAIR_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiChartCrosshairRegistry *registry, const char *id)
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
 * Initialise chart crosshair registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_crosshair_registry_create(UmiChartCrosshairRegistry **out_registry)
{
    UmiChartCrosshairRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartCrosshairRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by chart crosshair registry so the same storage can be
 * reused safely.
 */
void umi_chart_crosshair_registry_destroy(UmiChartCrosshairRegistry *registry)
{
    free(registry);
}

/*
 * Provide the chart crosshair registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_crosshair_registry_upsert(UmiChartCrosshairRegistry *registry, const UmiChartCrosshairSnapshot *item)
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
        if (registry->count >= UMI_CHART_CROSSHAIR_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiChartCrosshairSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove chart crosshair registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_crosshair_registry_remove(UmiChartCrosshairRegistry *registry, const char *id)
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
 * Find chart crosshair registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_crosshair_registry_find(const UmiChartCrosshairRegistry *registry, const char *id, UmiChartCrosshairSnapshot *out_item)
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
 * Find chart crosshair registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_crosshair_registry_at(const UmiChartCrosshairRegistry *registry, size_t index, UmiChartCrosshairSnapshot *out_item)
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
 * Return the number of records represented by chart crosshair registry without changing
 * their state.
 */
size_t umi_chart_crosshair_registry_count(const UmiChartCrosshairRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the chart crosshair registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_chart_crosshair_registry_revision(const UmiChartCrosshairRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
