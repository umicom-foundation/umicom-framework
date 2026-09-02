/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/widget_tree.c
 *
 * PURPOSE:
 *   Implement a frontend-neutral widget tree reusable by web, GTK and headless renderers.
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
#include "umicom/frontend/widget_tree.h"

#include <stdlib.h>
#include <string.h>

struct UmiFrontendWidgetRegistry {
    UmiFrontendWidgetSnapshot items[UMI_FRONTEND_WIDGET_TREE_CAPACITY];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiFrontendWidgetRegistry *registry, const char *id)
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
 * Initialise frontend widget tree registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_frontend_widget_tree_registry_create(UmiFrontendWidgetRegistry **out_registry)
{
    UmiFrontendWidgetRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiFrontendWidgetRegistry *)calloc(1U, sizeof(*registry));
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
 * Release or reset state held by frontend widget tree registry so the same storage can be
 * reused safely.
 */
void umi_frontend_widget_tree_registry_destroy(UmiFrontendWidgetRegistry *registry)
{
    free(registry);
}

/*
 * Provide the frontend widget tree registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_frontend_widget_tree_registry_upsert(UmiFrontendWidgetRegistry *registry, const UmiFrontendWidgetSnapshot *item)
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
        if (registry->count >= UMI_FRONTEND_WIDGET_TREE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiFrontendWidgetSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

/*
 * Remove frontend widget tree registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_frontend_widget_tree_registry_remove(UmiFrontendWidgetRegistry *registry, const char *id)
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
 * Find frontend widget tree registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_widget_tree_registry_find(const UmiFrontendWidgetRegistry *registry, const char *id, UmiFrontendWidgetSnapshot *out_item)
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
 * Find frontend widget tree registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_frontend_widget_tree_registry_at(const UmiFrontendWidgetRegistry *registry, size_t index, UmiFrontendWidgetSnapshot *out_item)
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
 * Return the number of records represented by frontend widget tree registry without
 * changing their state.
 */
size_t umi_frontend_widget_tree_registry_count(const UmiFrontendWidgetRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the frontend widget tree registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_frontend_widget_tree_registry_revision(const UmiFrontendWidgetRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
