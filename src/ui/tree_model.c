/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/tree_model.c
 *
 * PURPOSE:
 *   Implement a toolkit-neutral hierarchical tree model for Explorer-style views.
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
#include "umicom/ui/tree_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiUiTreeModelRegistry {
    UmiUiTreeModelSnapshot items[UMI_UI_TREE_MODEL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiUiTreeModelRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_tree_model_registry_create(UmiUiTreeModelRegistry **out_registry)
{
    UmiUiTreeModelRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiTreeModelRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_tree_model_registry_destroy(UmiUiTreeModelRegistry *registry)
{
    free(registry);
}

UmiStatus umi_ui_tree_model_registry_upsert(UmiUiTreeModelRegistry *registry, const UmiUiTreeModelSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_UI_TREE_MODEL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiUiTreeModelSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_tree_model_registry_remove(UmiUiTreeModelRegistry *registry, const char *id)
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

UmiStatus umi_ui_tree_model_registry_find(const UmiUiTreeModelRegistry *registry, const char *id, UmiUiTreeModelSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_tree_model_registry_at(const UmiUiTreeModelRegistry *registry, size_t index, UmiUiTreeModelSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_tree_model_registry_set_expanded(UmiUiTreeModelRegistry *registry,
                                                    const char *id,
                                                    int expanded)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    registry->items[index].expanded = expanded != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}

size_t umi_ui_tree_model_registry_child_count(const UmiUiTreeModelRegistry *registry,
                                              const char *parent_id)
{
    size_t index;
    size_t count = 0U;
    if (registry == NULL || parent_id == NULL) return 0U;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].parent_id, parent_id) == 0) count += 1U;
    }
    return count;
}

UmiStatus umi_ui_tree_model_registry_child_at(const UmiUiTreeModelRegistry *registry,
                                              const char *parent_id,
                                              size_t child_index,
                                              UmiUiTreeModelSnapshot *out_item)
{
    size_t index;
    size_t seen = 0U;
    if (registry == NULL || parent_id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].parent_id, parent_id) != 0) continue;
        if (seen == child_index) { *out_item = registry->items[index]; return UMI_STATUS_OK; }
        seen += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_ui_tree_model_registry_count(const UmiUiTreeModelRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_ui_tree_model_registry_revision(const UmiUiTreeModelRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
