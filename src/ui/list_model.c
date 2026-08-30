/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/list_model.c
 *
 * PURPOSE:
 *   Implement a toolkit-neutral observable list model for reusable workbench views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/ui/list_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiUiListModelRegistry {
    UmiUiListModelSnapshot items[UMI_UI_LIST_MODEL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiUiListModelRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_list_model_registry_create(UmiUiListModelRegistry **out_registry)
{
    UmiUiListModelRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiListModelRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_list_model_registry_destroy(UmiUiListModelRegistry *registry)
{
    free(registry);
}

UmiStatus umi_ui_list_model_registry_upsert(UmiUiListModelRegistry *registry, const UmiUiListModelSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_UI_LIST_MODEL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiUiListModelSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    registry->items[index].visible = registry->items[index].visible != 0; registry->items[index].enabled = registry->items[index].enabled != 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_list_model_registry_remove(UmiUiListModelRegistry *registry, const char *id)
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

UmiStatus umi_ui_list_model_registry_find(const UmiUiListModelRegistry *registry, const char *id, UmiUiListModelSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_list_model_registry_at(const UmiUiListModelRegistry *registry, size_t index, UmiUiListModelSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ui_list_model_registry_count(const UmiUiListModelRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_ui_list_model_registry_revision(const UmiUiListModelRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

UmiStatus umi_ui_list_model_registry_set_checked(UmiUiListModelRegistry *registry, const char *id, int checked)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    registry->items[index].checked = checked != 0;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    return UMI_STATUS_OK;
}
