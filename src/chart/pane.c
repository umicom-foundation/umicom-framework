/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/pane.c
 *
 * PURPOSE:
 *   Implement reusable chart panes so multiple price and indicator regions share one timeline.
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
#include "umicom/chart/pane.h"
#include "../base/value_archive_internal.h"
#include "../base/registry_archive_internal.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiChartPaneSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiChartPaneSnapshot, title, 0)
};
UmiStatus umi_chart_pane_snapshot_validate(const UmiChartPaneSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdlib.h>
#include <string.h>

struct UmiChartPaneRegistry {
    UmiChartPaneSnapshot items[UMI_CHART_PANE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiChartPaneRegistry *registry, const char *id)
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
 * Initialise chart pane registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_chart_pane_registry_create(UmiChartPaneRegistry **out_registry)
{
    UmiChartPaneRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiChartPaneRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by chart pane registry so the same storage can be reused
 * safely.
 */
void umi_chart_pane_registry_destroy(UmiChartPaneRegistry *registry)
{
    free(registry);
}

/*
 * Provide the chart pane registry upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_chart_pane_registry_upsert(UmiChartPaneRegistry *registry, const UmiChartPaneSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_chart_pane_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_CHART_PANE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiChartPaneSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove chart pane registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_chart_pane_registry_remove(UmiChartPaneRegistry *registry, const char *id)
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
 * Find chart pane registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_pane_registry_find(const UmiChartPaneRegistry *registry, const char *id, UmiChartPaneSnapshot *out_item)
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
 * Find chart pane registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_chart_pane_registry_at(const UmiChartPaneRegistry *registry, size_t index, UmiChartPaneSnapshot *out_item)
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
 * Return the number of records represented by chart pane registry without changing their
 * state.
 */
size_t umi_chart_pane_registry_count(const UmiChartPaneRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the chart pane registry revision operation used by this module and its client
 * applications.
 */
uint64_t umi_chart_pane_registry_revision(const UmiChartPaneRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_chart_pane_registry_upsert_many,
    UmiChartPaneRegistry, UmiChartPaneSnapshot,
    umi_chart_pane_snapshot_validate, umi_chart_pane_registry_upsert, UMI_CHART_PANE_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_chart_pane_registry_capture,
    umi_chart_pane_registry_replace_if_current, UmiChartPaneRegistry, UmiChartPaneSnapshot,
    umi_chart_pane_snapshot_validate, umi_chart_pane_registry_upsert, UMI_CHART_PANE_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_chart_pane_registry_edit_if_current,
    UmiChartPaneRegistry, UmiChartPaneSnapshot, UmiChartPaneEdit,
    umi_chart_pane_snapshot_validate, umi_chart_pane_registry_upsert, umi_chart_pane_registry_remove, UMI_CHART_PANE_CAPACITY)

/* Read accepted pane records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_chart_pane_registry_read_page,
    UmiChartPaneRegistry, UmiChartPaneSnapshot, UMI_CHART_PANE_CAPACITY)

/* Portable state belongs to the Framework owner. Enumerate fields explicitly
 * so saved bytes contain neither struct padding nor unused text. When adding
 * a field, extend both directions, the schema identity and the domain fixture;
 * incompatible layouts must be migrated deliberately before publication. */
static uint64_t ArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8702412a237a094c);
    schema = (schema ^ (uint64_t)sizeof(((UmiChartPaneSnapshot *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiChartPaneSnapshot *)0)->title)) * UINT64_C(1099511628211);
    return schema;
}
static size_t ArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U + sizeof(((UmiChartPaneSnapshot *)0)->id) - 1U +
        8U + sizeof(((UmiChartPaneSnapshot *)0)->title) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void ArchiveWriteFields(UmiArchiveWriter *writer, const UmiChartPaneSnapshot *value)
{
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->api_version);
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->title, sizeof(value->title));
    UmiArchiveWriteDouble(writer, value->height_weight);
    UmiArchiveWriteSigned(writer, (int64_t)value->visible);
    UmiArchiveWriteSigned(writer, (int64_t)value->collapsed);
    UmiArchiveWriteSigned(writer, (int64_t)value->order);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void ArchiveReadFields(UmiArchiveReader *reader, UmiChartPaneSnapshot *value)
{
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->title, sizeof(value->title));
    value->height_weight = UmiArchiveReadDouble(reader);
    value->visible = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->collapsed = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->order = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus ArchiveValidate(const UmiChartPaneSnapshot *value)
{
    return umi_chart_pane_snapshot_validate(value, NULL);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_chart_pane_snapshot_archive_encode, umi_chart_pane_snapshot_archive_decode,
    UmiChartPaneSnapshot, ArchiveSchema, ArchiveBound, ArchiveWriteFields, ArchiveReadFields, ArchiveValidate)

/* Restoring this complete collection reuses its existing reviewed replacement
 * rules. Decoding a saved value alone never changes a live registry. */
UMI_DEFINE_REGISTRY_ARCHIVE(umi_chart_pane_registry_archive_encode, umi_chart_pane_registry_archive_restore,
    UmiChartPaneRegistry, UmiChartPaneSnapshot, UMI_CHART_PANE_CAPACITY, ArchiveSchema,
    umi_chart_pane_snapshot_archive_encode, umi_chart_pane_snapshot_archive_decode, umi_chart_pane_registry_replace_if_current)
