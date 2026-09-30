/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/clipboard.c
 *
 * PURPOSE:
 *   Implement a toolkit-neutral designer clipboard for copy, cut, paste and duplication.
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
#include "umicom/designer/clipboard.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerClipboardItemSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerClipboardItemSnapshot, source_node_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerClipboardItemSnapshot, component_type, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerClipboardItemSnapshot, serialized, 0)
};
UmiStatus umi_designer_clipboard_snapshot_validate(const UmiDesignerClipboardItemSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdlib.h>
#include <string.h>

struct UmiDesignerClipboardItemRegistry {
    UmiDesignerClipboardItemSnapshot items[UMI_DESIGNER_CLIPBOARD_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDesignerClipboardItemRegistry *registry, const char *id)
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
 * Initialise designer clipboard registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_clipboard_registry_create(UmiDesignerClipboardItemRegistry **out_registry)
{
    UmiDesignerClipboardItemRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDesignerClipboardItemRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by designer clipboard registry so the same storage can be
 * reused safely.
 */
void umi_designer_clipboard_registry_destroy(UmiDesignerClipboardItemRegistry *registry)
{
    free(registry);
}

/*
 * Provide the designer clipboard registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_clipboard_registry_upsert(UmiDesignerClipboardItemRegistry *registry, const UmiDesignerClipboardItemSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_designer_clipboard_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_DESIGNER_CLIPBOARD_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiDesignerClipboardItemSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove designer clipboard registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_clipboard_registry_remove(UmiDesignerClipboardItemRegistry *registry, const char *id)
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
 * Find designer clipboard registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_clipboard_registry_find(const UmiDesignerClipboardItemRegistry *registry, const char *id, UmiDesignerClipboardItemSnapshot *out_item)
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
 * Find designer clipboard registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_clipboard_registry_at(const UmiDesignerClipboardItemRegistry *registry, size_t index, UmiDesignerClipboardItemSnapshot *out_item)
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
 * Return the number of records represented by designer clipboard registry without changing
 * their state.
 */
size_t umi_designer_clipboard_registry_count(const UmiDesignerClipboardItemRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the designer clipboard registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_clipboard_registry_revision(const UmiDesignerClipboardItemRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_designer_clipboard_registry_upsert_many,
    UmiDesignerClipboardItemRegistry, UmiDesignerClipboardItemSnapshot,
    umi_designer_clipboard_snapshot_validate, umi_designer_clipboard_registry_upsert, UMI_DESIGNER_CLIPBOARD_CAPACITY)
