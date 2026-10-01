/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/transport.c
 *
 * PURPOSE:
 *   Implement frontend transport state for WebSocket, event-stream and request/response delivery.
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
#include "umicom/frontend/transport.h"
#include "../base/snapshot_registry_internal.h"
/* Every field is described with its actual C member size; no string scan can
 * escape a supplied array. Domain values and legacy size/version normalisation
 * stay with the owning service. Diagnostics never copy a record's contents. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiFrontendTransportSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiFrontendTransportSnapshot, session_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiFrontendTransportSnapshot, kind, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiFrontendTransportSnapshot, endpoint, 0)
};

UmiStatus umi_frontend_transport_snapshot_validate(const UmiFrontendTransportSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdlib.h>
#include <string.h>

struct UmiFrontendTransportRegistry {
    UmiFrontendTransportSnapshot items[UMI_FRONTEND_TRANSPORT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendTransportRegistry *registry, const char *id)
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
 * Initialise frontend transport registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_transport_registry_create(UmiFrontendTransportRegistry **out_registry)
{
    UmiFrontendTransportRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiFrontendTransportRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by frontend transport registry so the same storage can be
 * reused safely.
 */
void umi_frontend_transport_registry_destroy(UmiFrontendTransportRegistry *registry)
{
    free(registry);
}

/*
 * Provide the frontend transport registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_transport_registry_upsert(UmiFrontendTransportRegistry *registry, const UmiFrontendTransportSnapshot *item)
{
    /* Validate before ID lookup or mutation. The previous copy/terminator
     * implementation remains below for review and for normalising valid input.
     * Malformed arrays are now rejected instead of scanned beyond their bounds
     * or silently truncated. Reusable enforcement belongs in Framework. */
    UmiStatus validation = umi_frontend_transport_snapshot_validate(item, NULL);
    if (validation != UMI_STATUS_OK) return validation;
    if (registry != NULL && registry->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
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
        if (registry->count >= UMI_FRONTEND_TRANSPORT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiFrontendTransportSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove frontend transport registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_transport_registry_remove(UmiFrontendTransportRegistry *registry, const char *id)
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
 * Find frontend transport registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_transport_registry_find(const UmiFrontendTransportRegistry *registry, const char *id, UmiFrontendTransportSnapshot *out_item)
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
 * Find frontend transport registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_transport_registry_at(const UmiFrontendTransportRegistry *registry, size_t index, UmiFrontendTransportSnapshot *out_item)
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
 * Return the number of records represented by frontend transport registry without changing
 * their state.
 */
size_t umi_frontend_transport_registry_count(const UmiFrontendTransportRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the frontend transport registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_transport_registry_revision(const UmiFrontendTransportRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

/* Stage a complete value-only registry before publishing a batch. Existing
 * upsert semantics run against the private copy, so any failed row leaves the
 * caller's count, records and revision unchanged. No application duplicate is
 * needed, and the original single-record implementation remains available. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_frontend_transport_registry_upsert_many,
    UmiFrontendTransportRegistry, UmiFrontendTransportSnapshot,
    umi_frontend_transport_snapshot_validate, umi_frontend_transport_registry_upsert, UMI_FRONTEND_TRANSPORT_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_frontend_transport_registry_capture,
    umi_frontend_transport_registry_replace_if_current, UmiFrontendTransportRegistry, UmiFrontendTransportSnapshot,
    umi_frontend_transport_snapshot_validate, umi_frontend_transport_registry_upsert, UMI_FRONTEND_TRANSPORT_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_frontend_transport_registry_edit_if_current,
    UmiFrontendTransportRegistry, UmiFrontendTransportSnapshot, UmiFrontendTransportEdit,
    umi_frontend_transport_snapshot_validate, umi_frontend_transport_registry_upsert, umi_frontend_transport_registry_remove, UMI_FRONTEND_TRANSPORT_CAPACITY)

/* Read accepted transport records in bounded pages. The shared
 * Framework rule refuses a changed observation before publishing output;
 * this owner's existing row order and normalized fields remain authoritative. */
UMI_DEFINE_SNAPSHOT_REGISTRY_PAGE(umi_frontend_transport_registry_read_page,
    UmiFrontendTransportRegistry, UmiFrontendTransportSnapshot, UMI_FRONTEND_TRANSPORT_CAPACITY)
