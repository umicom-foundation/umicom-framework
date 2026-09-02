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

/*
 * Initialise debug workbench watch expression item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_watch_expression_item_init(UmiDebugWorkbenchWatchExpressionItem *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, label, detail, path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench watch expression item set state operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_watch_expression_item_set_state(UmiDebugWorkbenchWatchExpressionItem *model, uint32_t state, uint64_t value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->value.state = state;
    model->value.value = value;
    model->value.revision++;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Find debug workbench watch expression item set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_watch_expression_item_set_selected(UmiDebugWorkbenchWatchExpressionItem *model, bool selected)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->selected = selected;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench watch expression item set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_watch_expression_item_set_enabled(UmiDebugWorkbenchWatchExpressionItem *model, bool enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    model->enabled = enabled;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench watch expression item satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_watch_expression_item_valid(const UmiDebugWorkbenchWatchExpressionItem *model)
{
    return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->revision > 0U;
}
