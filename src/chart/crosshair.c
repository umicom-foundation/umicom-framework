/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/crosshair.c
 *
 * PURPOSE:
 *   Implement synchronised crosshair state shared across chart panes and series.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

static size_t find_index(const UmiChartCrosshairRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_chart_crosshair_registry_create(UmiChartCrosshairRegistry **out_registry)
{
    UmiChartCrosshairRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartCrosshairRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_chart_crosshair_registry_destroy(UmiChartCrosshairRegistry *registry)
{
    free(registry);
}

UmiStatus umi_chart_crosshair_registry_upsert(UmiChartCrosshairRegistry *registry, const UmiChartCrosshairSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
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

UmiStatus umi_chart_crosshair_registry_remove(UmiChartCrosshairRegistry *registry, const char *id)
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

UmiStatus umi_chart_crosshair_registry_find(const UmiChartCrosshairRegistry *registry, const char *id, UmiChartCrosshairSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_chart_crosshair_registry_at(const UmiChartCrosshairRegistry *registry, size_t index, UmiChartCrosshairSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_chart_crosshair_registry_count(const UmiChartCrosshairRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_chart_crosshair_registry_revision(const UmiChartCrosshairRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
