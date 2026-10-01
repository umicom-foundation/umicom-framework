/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing.c
 *
 * PURPOSE:
 *   Implement persistent drawing-tool geometry for trend lines, ranges and measurement tools.
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
#include "umicom/chart/drawing.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiChartDrawingSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiChartDrawingSnapshot, pane_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiChartDrawingSnapshot, tool, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiChartDrawingSnapshot, style, 0)
};
UmiStatus umi_chart_drawing_snapshot_validate(const UmiChartDrawingSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdlib.h>
#include <string.h>

struct UmiChartDrawingRegistry {
    UmiChartDrawingSnapshot items[UMI_CHART_DRAWING_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiChartDrawingRegistry *registry, const char *id)
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
 * Initialise chart drawing registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_chart_drawing_registry_create(UmiChartDrawingRegistry **out_registry)
{
    UmiChartDrawingRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartDrawingRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by chart drawing registry so the same storage can be reused
 * safely.
 */
void umi_chart_drawing_registry_destroy(UmiChartDrawingRegistry *registry)
{
    free(registry);
}

/*
 * Provide the chart drawing registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_drawing_registry_upsert(UmiChartDrawingRegistry *registry, const UmiChartDrawingSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_chart_drawing_snapshot_validate(item, NULL);
    if (validation != UMI_STATUS_OK) return validation;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

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
        if (registry->count >= UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiChartDrawingSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove chart drawing registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_drawing_registry_remove(UmiChartDrawingRegistry *registry, const char *id)
{
    /* A wrapped revision could make an old edit appear current again. Refuse
     * mutation before touching records when no fresh revision is available. */
    if (registry != NULL && registry->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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
 * Find chart drawing registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_drawing_registry_find(const UmiChartDrawingRegistry *registry, const char *id, UmiChartDrawingSnapshot *out_item)
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
 * Find chart drawing registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_chart_drawing_registry_at(const UmiChartDrawingRegistry *registry, size_t index, UmiChartDrawingSnapshot *out_item)
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
 * Return the number of records represented by chart drawing registry without changing
 * their state.
 */
size_t umi_chart_drawing_registry_count(const UmiChartDrawingRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the chart drawing registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_drawing_registry_revision(const UmiChartDrawingRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_chart_drawing_registry_upsert_many,
    UmiChartDrawingRegistry, UmiChartDrawingSnapshot,
    umi_chart_drawing_snapshot_validate, umi_chart_drawing_registry_upsert, UMI_CHART_DRAWING_CAPACITY)

/* Persistence publishes a fully validated candidate into the established
 * registry. The registry object and unrelated pane records keep their owners;
 * a failed restore cannot leave half of a pane removed or imported. */
#include "umicom/chart/drawing_validation.h"
UmiStatus UmiChartDrawingRegistryReplacePane(UmiChartDrawingRegistry *registry,
    const char *paneId, const UmiChartDrawingSnapshot *items, size_t count,
    uint64_t expectedRevision)
{
    if (registry == NULL || paneId == NULL || paneId[0] == '\0' ||
        (items == NULL && count != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t paneLength = 0U;
    while (paneLength < sizeof(((UmiChartDrawingSnapshot *)0)->pane_id) && paneId[paneLength] != '\0') ++paneLength;
    if (paneLength == sizeof(((UmiChartDrawingSnapshot *)0)->pane_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_INVALID_STATE;
    if (count > UMI_CHART_DRAWING_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (registry->revision != expectedRevision) return UMI_STATUS_INVALID_STATE;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t retained = 0U;
    for (size_t i = 0U; i < registry->count; ++i)
        if (strcmp(registry->items[i].pane_id, paneId) != 0) ++retained;
    if (count > UMI_CHART_DRAWING_CAPACITY - retained) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = UmiChartDrawingValidateGeometry(&items[i]);
        if (status != UMI_STATUS_OK) return status;
        if (strcmp(items[i].pane_id, paneId) != 0) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(items[i].id, items[j].id) == 0) return UMI_STATUS_ALREADY_EXISTS;
        size_t existing = find_index(registry, items[i].id);
        if (existing != SIZE_MAX && strcmp(registry->items[existing].pane_id, paneId) != 0)
            return UMI_STATUS_ALREADY_EXISTS;
    }
    UmiChartDrawingRegistry *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->revision = registry->revision + 1U;
    for (size_t i = 0U; i < registry->count; ++i)
        if (strcmp(registry->items[i].pane_id, paneId) != 0) candidate->items[candidate->count++] = registry->items[i];
    for (size_t i = 0U; i < count; ++i) {
        UmiChartDrawingSnapshot *record = &candidate->items[candidate->count++];
        *record = items[i]; record->struct_size = (uint32_t)sizeof(*record);
        record->api_version = 1U; record->revision = candidate->revision;
    }
    *registry = *candidate; free(candidate); return UMI_STATUS_OK;
}


/* History needs one atomic registry publication. Keeping its implementation
 * with this owner avoids exposing writable registry internals to applications. */
#include "drawing_history.inc"

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_chart_drawing_registry_capture,
    umi_chart_drawing_registry_replace_if_current, UmiChartDrawingRegistry, UmiChartDrawingSnapshot,
    umi_chart_drawing_snapshot_validate, umi_chart_drawing_registry_upsert, UMI_CHART_DRAWING_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_chart_drawing_registry_edit_if_current,
    UmiChartDrawingRegistry, UmiChartDrawingSnapshot, UmiChartDrawingEdit,
    umi_chart_drawing_snapshot_validate, umi_chart_drawing_registry_upsert, umi_chart_drawing_registry_remove, UMI_CHART_DRAWING_CAPACITY)
