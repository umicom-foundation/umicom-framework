/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/alignment.c
 *
 * PURPOSE:
 *   Implement deterministic alignment and distribution operations for visual design surfaces.
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
#include "umicom/designer/alignment.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerAlignmentRegistry {
    UmiDesignerAlignmentSnapshot items[UMI_DESIGNER_ALIGNMENT_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDesignerAlignmentRegistry *registry, const char *id)
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
 * Initialise designer alignment registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_alignment_registry_create(UmiDesignerAlignmentRegistry **out_registry)
{
    UmiDesignerAlignmentRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiDesignerAlignmentRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by designer alignment registry so the same storage can be
 * reused safely.
 */
void umi_designer_alignment_registry_destroy(UmiDesignerAlignmentRegistry *registry)
{
    free(registry);
}

/*
 * Provide the designer alignment registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_alignment_registry_upsert(UmiDesignerAlignmentRegistry *registry, const UmiDesignerAlignmentSnapshot *item)
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
        if (registry->count >= UMI_DESIGNER_ALIGNMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiDesignerAlignmentSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove designer alignment registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_designer_alignment_registry_remove(UmiDesignerAlignmentRegistry *registry, const char *id)
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
 * Find designer alignment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_alignment_registry_find(const UmiDesignerAlignmentRegistry *registry, const char *id, UmiDesignerAlignmentSnapshot *out_item)
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
 * Find designer alignment registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_designer_alignment_registry_at(const UmiDesignerAlignmentRegistry *registry, size_t index, UmiDesignerAlignmentSnapshot *out_item)
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
 * Return the number of records represented by designer alignment registry without changing
 * their state.
 */
size_t umi_designer_alignment_registry_count(const UmiDesignerAlignmentRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the designer alignment registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_designer_alignment_registry_revision(const UmiDesignerAlignmentRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
