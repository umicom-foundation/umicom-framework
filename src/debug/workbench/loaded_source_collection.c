/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/loaded_source_collection.c
 *
 * PURPOSE:
 *   Maintain loaded-source ordering and source-selection state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/loaded_source_collection.h"

#include <string.h>

void umi_debug_workbench_loaded_source_collection_init(UmiDebugWorkbenchLoadedSourceCollection *model)
{
    if (model == NULL) return;
    memset(model, 0, sizeof *model);
    model->active_index = UMI_DEBUG_WORKBENCH_NO_INDEX;
    model->revision = 1U;
}

const UmiDebugWorkbenchEntry *umi_debug_workbench_loaded_source_collection_find(const UmiDebugWorkbenchLoadedSourceCollection *model, const char *id)
{
    size_t index;
    if (model == NULL || !umi_debug_workbench_id_valid(id)) return NULL;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].id, id) == 0) return &model->items[index];
    }
    return NULL;
}

UmiStatus umi_debug_workbench_loaded_source_collection_add(UmiDebugWorkbenchLoadedSourceCollection *model, const UmiDebugWorkbenchEntry *entry)
{
    if (model == NULL || !umi_debug_workbench_entry_valid(entry)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_debug_workbench_loaded_source_collection_find(model, entry->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (model->count >= UMI_DEBUG_WORKBENCH_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->items[model->count] = *entry;
    model->count++;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_loaded_source_collection_remove(UmiDebugWorkbenchLoadedSourceCollection *model, const char *id)
{
    size_t index;
    if (model == NULL || !umi_debug_workbench_id_valid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].id, id) == 0) {
            size_t tail = model->count - index - 1U;
            if (tail > 0U) memmove(&model->items[index], &model->items[index + 1U], tail * sizeof model->items[0]);
            model->count--;
            if (model->active_index == index) model->active_index = UMI_DEBUG_WORKBENCH_NO_INDEX;
            else if (model->active_index != UMI_DEBUG_WORKBENCH_NO_INDEX && model->active_index > index) model->active_index--;
            model->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_debug_workbench_loaded_source_collection_select(UmiDebugWorkbenchLoadedSourceCollection *model, const char *id)
{
    size_t index;
    if (model == NULL || !umi_debug_workbench_id_valid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < model->count; ++index) {
        if (strcmp(model->items[index].id, id) == 0) {
            model->active_index = index;
            model->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

int umi_debug_workbench_loaded_source_collection_valid(const UmiDebugWorkbenchLoadedSourceCollection *model)
{
    size_t index;
    if (model == NULL || model->count > UMI_DEBUG_WORKBENCH_MAX_ITEMS || model->revision == 0U) return 0;
    if (model->active_index != UMI_DEBUG_WORKBENCH_NO_INDEX && model->active_index >= model->count) return 0;
    for (index = 0U; index < model->count; ++index) if (!umi_debug_workbench_entry_valid(&model->items[index])) return 0;
    return 1;
}
