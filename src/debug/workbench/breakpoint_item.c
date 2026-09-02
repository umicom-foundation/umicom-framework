/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/breakpoint_item.c
 *
 * PURPOSE:
 *   Represent one source, function, data, exception, instruction or log breakpoint.
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
#include "umicom/debug/workbench/breakpoint_item.h"
#include <string.h>

/*
 * Initialise debug workbench breakpoint item from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_breakpoint_item_init(UmiDebugWorkbenchBreakpointItem *model, const char *id, UmiDebugWorkbenchBreakpointKind kind, const char *path, UmiDebugWorkbenchRange range)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || kind < UMI_DEBUG_WORKBENCH_BREAKPOINT_SOURCE || kind > UMI_DEBUG_WORKBENCH_BREAKPOINT_LOGPOINT) return UMI_STATUS_INVALID_ARGUMENT;
    memset(model, 0, sizeof *model);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_debug_workbench_entry_init(&model->value, id, "Breakpoint", "", path, range) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    model->kind = kind;
    model->enabled = true;
    model->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench breakpoint item set verification operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_item_set_verification(UmiDebugWorkbenchBreakpointItem *model, bool verified)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; model->verified = verified; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench breakpoint item record hit operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_item_record_hit(UmiDebugWorkbenchBreakpointItem *model)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; model->hit_count++; model->revision++; return UMI_STATUS_OK; }
/*
 * Provide the debug workbench breakpoint item set enabled operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_breakpoint_item_set_enabled(UmiDebugWorkbenchBreakpointItem *model, bool enabled)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT; model->enabled = enabled; model->revision++; return UMI_STATUS_OK; }
/*
 * Check that debug workbench breakpoint item satisfies its contract before another service
 * relies on it.
 */
int umi_debug_workbench_breakpoint_item_valid(const UmiDebugWorkbenchBreakpointItem *model)
{ return model != NULL && umi_debug_workbench_entry_valid(&model->value) && model->kind >= UMI_DEBUG_WORKBENCH_BREAKPOINT_SOURCE && model->kind <= UMI_DEBUG_WORKBENCH_BREAKPOINT_LOGPOINT && model->revision > 0U; }
