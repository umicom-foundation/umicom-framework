/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/annotation.c
 *
 * PURPOSE:
 *   Implement user and system annotations without coupling chart data to drawing widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/chart/annotation.h"

#include <stdlib.h>
#include <string.h>

struct UmiChartAnnotationRegistry {
    UmiChartAnnotationSnapshot items[UMI_CHART_ANNOTATION_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiChartAnnotationRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_chart_annotation_registry_create(UmiChartAnnotationRegistry **out_registry)
{
    UmiChartAnnotationRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartAnnotationRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_chart_annotation_registry_destroy(UmiChartAnnotationRegistry *registry)
{
    free(registry);
}

UmiStatus umi_chart_annotation_registry_upsert(UmiChartAnnotationRegistry *registry, const UmiChartAnnotationSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_CHART_ANNOTATION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiChartAnnotationSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_chart_annotation_registry_remove(UmiChartAnnotationRegistry *registry, const char *id)
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

UmiStatus umi_chart_annotation_registry_find(const UmiChartAnnotationRegistry *registry, const char *id, UmiChartAnnotationSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_chart_annotation_registry_at(const UmiChartAnnotationRegistry *registry, size_t index, UmiChartAnnotationSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_chart_annotation_registry_count(const UmiChartAnnotationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_chart_annotation_registry_revision(const UmiChartAnnotationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
