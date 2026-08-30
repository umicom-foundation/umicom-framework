/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/watch_expression_item.c
 *
 * PURPOSE:
 *   Represent one persisted watch expression and latest evaluation status.
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
#include "umicom/debug/workbench/watch_expression_item.h"

#include <string.h>

UmiStatus umi_debug_workbench_watch_expression_item_init(UmiDebugWorkbenchWatchExpressionItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    if (umi_debug_workbench_entry_init(&model->value, id, label, detail, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_watch_expression_item_set_state(UmiDebugWorkbenchWatchExpressionItem *model, uint32_t state, uint64_t value)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->value.state = state;
    model->value.value = value;
    model->value.revision++;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_watch_expression_item_set_selected(UmiDebugWorkbenchWatchExpressionItem *model, bool selected)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->selected = selected;
    model->revision++;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_workbench_watch_expression_item_set_enabled(UmiDebugWorkbenchWatchExpressionItem *model, bool enabled)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = enabled;
    model->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_watch_expression_item_valid(const UmiDebugWorkbenchWatchExpressionItem *model)
{
    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}
