/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/stream.c
 *
 * PURPOSE:
 *   Implement live streaming state and counters for high-frequency chart updates.
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
#include "umicom/chart/stream.h"

#include <stdlib.h>
#include <string.h>

struct UmiChartStreamRegistry {
    UmiChartStreamSnapshot items[UMI_CHART_STREAM_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiChartStreamRegistry *registry, const char *id)
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
 * Initialise chart stream registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_chart_stream_registry_create(UmiChartStreamRegistry **out_registry)
{
    UmiChartStreamRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartStreamRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by chart stream registry so the same storage can be reused
 * safely.
 */
void umi_chart_stream_registry_destroy(UmiChartStreamRegistry *registry)
{
    free(registry);
}

/*
 * Provide the chart stream registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_upsert(UmiChartStreamRegistry *registry, const UmiChartStreamSnapshot *item)
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
        if (registry->count >= UMI_CHART_STREAM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiChartStreamSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove chart stream registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_stream_registry_remove(UmiChartStreamRegistry *registry, const char *id)
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
 * Find chart stream registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_stream_registry_find(const UmiChartStreamRegistry *registry, const char *id, UmiChartStreamSnapshot *out_item)
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
 * Find chart stream registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_stream_registry_at(const UmiChartStreamRegistry *registry, size_t index, UmiChartStreamSnapshot *out_item)
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
 * Provide the chart stream registry record operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_record(UmiChartStreamRegistry *registry,
                                             const char *id,
                                             int64_t time,
                                             double value,
                                             int dropped_update)
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
    /* Apply this branch only when its contract condition is satisfied. */
    if (dropped_update) {
        registry->items[index].dropped += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        registry->items[index].updates += 1U;
        registry->items[index].last_time = time;
        registry->items[index].last_value = value;
    }
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the chart stream registry set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_stream_registry_set_state(UmiChartStreamRegistry *registry,
                                              const char *id,
                                              int connected,
                                              int paused)
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
    registry->items[index].connected = connected != 0;
    registry->items[index].paused = paused != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by chart stream registry without changing their
 * state.
 */
size_t umi_chart_stream_registry_count(const UmiChartStreamRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the chart stream registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_stream_registry_revision(const UmiChartStreamRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
