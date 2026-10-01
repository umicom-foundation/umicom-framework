/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_control/change.c
 *
 * PURPOSE:
 *   Implement a provider-neutral source-control workspace record above the low-level VCS adapter boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/source_control/change.h"
#include "../base/snapshot_registry_internal.h"
/* Every field is described with its actual C member size; no string scan can
 * escape a supplied array. Domain values and legacy size/version normalisation
 * stay with the owning service. Diagnostics never copy a record's contents. */
static const UmiSnapshotTextField snapshot_text_fields[] = {
    UMI_SNAPSHOT_TEXT_FIELD(UmiSourceControlChangeSnapshot, id, 1),
    UMI_SNAPSHOT_TEXT_FIELD(UmiSourceControlChangeSnapshot, repository_id, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiSourceControlChangeSnapshot, uri, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiSourceControlChangeSnapshot, status, 0),
    UMI_SNAPSHOT_TEXT_FIELD(UmiSourceControlChangeSnapshot, old_uri, 0)
};

UmiStatus umi_source_control_change_snapshot_validate(const UmiSourceControlChangeSnapshot *item,
    UmiSnapshotValidation *outValidation)
{
    return UmiSnapshotValidateTextFields(item, sizeof(*item), snapshot_text_fields,
        sizeof(snapshot_text_fields) / sizeof(snapshot_text_fields[0]), outValidation);
}


#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiSourceControlChangeRegistry {
    UmiSourceControlChangeSnapshot items[UMI_SOURCE_CONTROL_CHANGE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiSourceControlChangeRegistry *registry, const char *id)
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
 * Initialise source control change registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_source_control_change_registry_create(UmiSourceControlChangeRegistry **out_registry)
{
    UmiSourceControlChangeRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiSourceControlChangeRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by source control change registry so the same storage can be
 * reused safely.
 */
void umi_source_control_change_registry_destroy(UmiSourceControlChangeRegistry *registry) { free(registry); }

/*
 * Provide the source control change registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_change_registry_upsert(UmiSourceControlChangeRegistry *registry, const UmiSourceControlChangeSnapshot *item)
{
    /* Validate before ID lookup or mutation. The previous copy/terminator
     * implementation remains below for review and for normalising valid input.
     * Malformed arrays are now rejected instead of scanned beyond their bounds
     * or silently truncated. Reusable enforcement belongs in Framework. */
    UmiStatus validation = umi_source_control_change_snapshot_validate(item, NULL);
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
        if (registry->count >= UMI_SOURCE_CONTROL_CHANGE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(registry->items[index]);
    registry->items[index].api_version = UMI_SOURCE_CONTROL_CHANGE_API_VERSION;
    registry->items[index].id[127U] = '\0';
    registry->items[index].repository_id[127U] = '\0';
    registry->items[index].uri[1023U] = '\0';
    registry->items[index].status[63U] = '\0';
    registry->items[index].old_uri[1023U] = '\0';
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove source control change registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_source_control_change_registry_remove(UmiSourceControlChangeRegistry *registry, const char *id)
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
                (registry->count-index-1U)*sizeof(registry->items[0]));
    }
    registry->count -= 1U; registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find source control change registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_change_registry_find(const UmiSourceControlChangeRegistry *registry, const char *id, UmiSourceControlChangeSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry,id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Find source control change registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_source_control_change_registry_at(const UmiSourceControlChangeRegistry *registry, size_t index, UmiSourceControlChangeSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index]; return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by source control change registry without
 * changing their state.
 */
size_t umi_source_control_change_registry_count(const UmiSourceControlChangeRegistry *registry) { return registry != NULL ? registry->count : 0U; }
/*
 * Provide the source control change registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_source_control_change_registry_revision(const UmiSourceControlChangeRegistry *registry) { return registry != NULL ? registry->revision : 0U; }
/*
 * Release or reset state held by source control change registry so the same storage can be
 * reused safely.
 */
void umi_source_control_change_registry_clear(UmiSourceControlChangeRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry->items,0,sizeof(registry->items)); registry->count=0U; registry->revision += 1U;
}

/* Stage a complete value-only registry before publishing a batch. Existing
 * upsert semantics run against the private copy, so any failed row leaves the
 * caller's count, records and revision unchanged. No application duplicate is
 * needed, and the original single-record implementation remains available. */
UMI_DEFINE_SNAPSHOT_REGISTRY_BATCH(umi_source_control_change_registry_upsert_many,
    UmiSourceControlChangeRegistry, UmiSourceControlChangeSnapshot,
    umi_source_control_change_snapshot_validate, umi_source_control_change_registry_upsert, UMI_SOURCE_CONTROL_CHANGE_CAPACITY)

/* A captured collection can be reviewed or prepared elsewhere, then published
 * on its owner only if no intervening edit changed this registry. Shared
 * Framework staging preserves the existing field validation and normalisation.
 * Extend the snapshot validator when adding fields; keep this owner value-only. */
UMI_DEFINE_SNAPSHOT_REGISTRY_TRANSFER(umi_source_control_change_registry_capture,
    umi_source_control_change_registry_replace_if_current, UmiSourceControlChangeRegistry, UmiSourceControlChangeSnapshot,
    umi_source_control_change_snapshot_validate, umi_source_control_change_registry_upsert, UMI_SOURCE_CONTROL_CHANGE_CAPACITY)

/* Share reviewed insertion/removal publication with other value registries.
 * Domain-specific field rules stay in this owner's existing operations. */
UMI_DEFINE_SNAPSHOT_REGISTRY_EDIT(umi_source_control_change_registry_edit_if_current,
    UmiSourceControlChangeRegistry, UmiSourceControlChangeSnapshot, UmiSourceControlChangeEdit,
    umi_source_control_change_snapshot_validate, umi_source_control_change_registry_upsert, umi_source_control_change_registry_remove, UMI_SOURCE_CONTROL_CHANGE_CAPACITY)
