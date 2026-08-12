/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/selection_model.c
 *
 * PURPOSE:
 *   Implement reusable selection, focus and anchor state independent of a GUI toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/ui/selection_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiUiSelectionModelRegistry {
    UmiUiSelectionModelSnapshot items[UMI_UI_SELECTION_MODEL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiUiSelectionModelRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_selection_model_registry_create(UmiUiSelectionModelRegistry **out_registry)
{
    UmiUiSelectionModelRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiSelectionModelRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_selection_model_registry_destroy(UmiUiSelectionModelRegistry *registry)
{
    free(registry);
}

UmiStatus umi_ui_selection_model_registry_upsert(UmiUiSelectionModelRegistry *registry, const UmiUiSelectionModelSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_UI_SELECTION_MODEL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiUiSelectionModelSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_selection_model_registry_remove(UmiUiSelectionModelRegistry *registry, const char *id)
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

UmiStatus umi_ui_selection_model_registry_find(const UmiUiSelectionModelRegistry *registry, const char *id, UmiUiSelectionModelSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_selection_model_registry_at(const UmiUiSelectionModelRegistry *registry, size_t index, UmiUiSelectionModelSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

size_t umi_ui_selection_model_registry_count(const UmiUiSelectionModelRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_ui_selection_model_registry_revision(const UmiUiSelectionModelRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}

UmiStatus umi_ui_selection_model_registry_clear(UmiUiSelectionModelRegistry *registry)
{
    size_t i;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i=0U;i<registry->count;++i) { registry->items[i].selected=0; registry->items[i].focused=0; registry->items[i].anchor=0; }
    registry->revision += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_ui_selection_model_registry_select_only(UmiUiSelectionModelRegistry *registry, const char *id)
{
    size_t i,index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index=find_index(registry,id); if(index==SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    for(i=0U;i<registry->count;++i){registry->items[i].selected=(i==index);registry->items[i].focused=(i==index);registry->items[i].anchor=(i==index);}
    registry->revision += 1U; registry->items[index].revision=registry->revision; return UMI_STATUS_OK;
}
