/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_configuration_registry.c
 *
 * PURPOSE:
 *   Maintain deterministic launch and attach configuration ordering and selection.
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
#include "umicom/debug/workbench/debug_configuration_registry.h"

#include <string.h>

/*
 * Initialise debug workbench debug configuration registry from caller-provided values so
 * later operations receive a known state.
 */
void umi_debug_workbench_debug_configuration_registry_init(UmiDebugWorkbenchDebugConfigurationRegistry *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    memset(model, 0, sizeof *model);
    model->active_index = UMI_DEBUG_WORKBENCH_NO_INDEX;
    model->revision = 1U;
}

/*
 * Find debug workbench debug configuration registry while leaving the underlying catalogue
 * or model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_configuration_registry_find(const UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_id_valid(id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].id, id) == 0) return &model->items[index];
    }
    return NULL;
}

/*
 * Add debug workbench debug configuration registry only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_debug_workbench_debug_configuration_registry_add(UmiDebugWorkbenchDebugConfigurationRegistry *model, const UmiDebugWorkbenchEntry *entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_entry_valid(entry)) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_debug_workbench_debug_configuration_registry_find(model, entry->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_DEBUG_WORKBENCH_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    model->items[model->count] = *entry;
    model->count++;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Remove debug workbench debug configuration registry while keeping the remaining records
 * in a valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_configuration_registry_remove(UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_id_valid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].id, id) == 0) {
            size_t tail = model->count - index - 1U;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (tail > 0U) memmove(&model->items[index], &model->items[index + 1U], tail * sizeof model->items[0]);
            model->count--;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (model->active_index == index) model->active_index = UMI_DEBUG_WORKBENCH_NO_INDEX;
            else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (model->active_index != UMI_DEBUG_WORKBENCH_NO_INDEX && model->active_index > index) model->active_index--;
            model->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the debug workbench debug configuration registry select operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_configuration_registry_select(UmiDebugWorkbenchDebugConfigurationRegistry *model, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_debug_workbench_id_valid(id)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(model->items[index].id, id) == 0) {
            model->active_index = index;
            model->revision++;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Check that debug workbench debug configuration registry satisfies its contract before
 * another service relies on it.
 */
int umi_debug_workbench_debug_configuration_registry_valid(const UmiDebugWorkbenchDebugConfigurationRegistry *model)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || model->count > UMI_DEBUG_WORKBENCH_MAX_ITEMS || model->revision == 0U) return 0;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->active_index != UMI_DEBUG_WORKBENCH_NO_INDEX && model->active_index >= model->count) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!umi_debug_workbench_entry_valid(&model->items[index])) return 0;
    return 1;
}
