/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/signal_binding.c
 *
 * PURPOSE:
 *   Implement visual-designer signal bindings without embedding toolkit callbacks in project files.
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
#include "umicom/designer/signal_binding.h"
#include "../base/snapshot_registry_internal.h"

/* Validate every bounded text member before lookup. Value-only snapshot
 * ownership stays with this existing Framework registry; domain semantics and
 * normalisation remain in its established implementation. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerSignalBindingSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerSignalBindingSnapshot, node_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerSignalBindingSnapshot, signal_name, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerSignalBindingSnapshot, command_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiDesignerSignalBindingSnapshot, argument, 0)
};
UmiStatus umi_designer_signal_binding_snapshot_validate(const UmiDesignerSignalBindingSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdlib.h>
#include <string.h>

struct UmiDesignerSignalBindingRegistry {
    UmiDesignerSignalBindingSnapshot items[UMI_DESIGNER_SIGNAL_BINDING_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDesignerSignalBindingRegistry *registry, const char *id)
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
 * Initialise designer signal binding registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_designer_signal_binding_registry_create(UmiDesignerSignalBindingRegistry **out_registry)
{
    UmiDesignerSignalBindingRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDesignerSignalBindingRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by designer signal binding registry so the same storage can
 * be reused safely.
 */
void umi_designer_signal_binding_registry_destroy(UmiDesignerSignalBindingRegistry *registry)
{
    free(registry);
}

/*
 * Provide the designer signal binding registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_designer_signal_binding_registry_upsert(UmiDesignerSignalBindingRegistry *registry, const UmiDesignerSignalBindingSnapshot *item)
{
    /* Central bounded validation rejects malformed snapshots before identity
     * comparison or mutation. The existing single-record logic is preserved;
     * valid records keep the same order, metadata and revision behavior. */
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus validation = umi_designer_signal_binding_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_DESIGNER_SIGNAL_BINDING_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiDesignerSignalBindingSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove designer signal binding registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_designer_signal_binding_registry_remove(UmiDesignerSignalBindingRegistry *registry, const char *id)
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
 * Find designer signal binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_signal_binding_registry_find(const UmiDesignerSignalBindingRegistry *registry, const char *id, UmiDesignerSignalBindingSnapshot *out_item)
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
 * Find designer signal binding registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_designer_signal_binding_registry_at(const UmiDesignerSignalBindingRegistry *registry, size_t index, UmiDesignerSignalBindingSnapshot *out_item)
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
 * Return the number of records represented by designer signal binding registry without
 * changing their state.
 */
size_t umi_designer_signal_binding_registry_count(const UmiDesignerSignalBindingRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the designer signal binding registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_designer_signal_binding_registry_revision(const UmiDesignerSignalBindingRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* A complete private value registry makes a multi-record import atomic on
 * the owner's thread. The original single-record API remains the authority
 * for valid record normalisation; no application-side registry is introduced. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_designer_signal_binding_registry_upsert_many,
    UmiDesignerSignalBindingRegistry, UmiDesignerSignalBindingSnapshot,
    umi_designer_signal_binding_snapshot_validate, umi_designer_signal_binding_registry_upsert, UMI_DESIGNER_SIGNAL_BINDING_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_designer_signal_binding_registry_capture,
    umi_designer_signal_binding_registry_replace_if_current, UmiDesignerSignalBindingRegistry, UmiDesignerSignalBindingSnapshot,
    umi_designer_signal_binding_snapshot_validate, umi_designer_signal_binding_registry_upsert, UMI_DESIGNER_SIGNAL_BINDING_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_designer_signal_binding_registry_edit_if_current,
    UmiDesignerSignalBindingRegistry, UmiDesignerSignalBindingSnapshot, UmiDesignerSignalBindingEdit,
    umi_designer_signal_binding_snapshot_validate, umi_designer_signal_binding_registry_upsert, umi_designer_signal_binding_registry_remove, UMI_DESIGNER_SIGNAL_BINDING_CAPACITY)
