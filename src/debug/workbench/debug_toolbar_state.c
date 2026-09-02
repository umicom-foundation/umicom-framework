/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_toolbar_state.c
 *
 * PURPOSE:
 *   Model debugger toolbar command availability independently of any GUI toolkit.
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
#include "umicom/debug/workbench/debug_toolbar_state.h"
#include <string.h>

/*
 * Initialise debug workbench debug toolbar state from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_toolbar_state_init(UmiDebugWorkbenchDebugToolbarState *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    memset(model, 0, sizeof *model);
    model->primary_command = UMI_DEBUG_WORKBENCH_COMMAND_START;
    model->visible_commands = (umi_debug_workbench_command_bit(UMI_DEBUG_WORKBENCH_COMMAND_COUNT - 1) << 1U) - 1U;
    model->revision = 1U;
}

/*
 * Provide the debug workbench debug toolbar state set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_toolbar_state_set_enabled(UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command, bool enabled)
{
    uint64_t bit;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bit = umi_debug_workbench_command_bit(command);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this operation only while the related capability or state is available. */
    if (enabled) model->enabled_commands |= bit; /* Use this fallback path when the earlier condition does not apply. */ else model->enabled_commands &= ~bit;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Provide the debug workbench debug toolbar state is enabled operation used by this module
 * and its client applications.
 */
int umi_debug_workbench_debug_toolbar_state_is_enabled(const UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command)
{
    uint64_t bit = umi_debug_workbench_command_bit(command);
    return model != NULL && bit != 0U && (model->enabled_commands & bit) != 0U;
}

/*
 * Provide the debug workbench debug toolbar state set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_toolbar_state_set_primary(UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || umi_debug_workbench_command_bit(command) == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    model->primary_command = command;
    model->revision++;
    return UMI_STATUS_OK;
}

/*
 * Check that debug workbench debug toolbar state satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_toolbar_state_valid(const UmiDebugWorkbenchDebugToolbarState *model)
{
    return model != NULL && model->revision > 0U && umi_debug_workbench_command_bit(model->primary_command) != 0U;
}
