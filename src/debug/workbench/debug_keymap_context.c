/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_keymap_context.c
 *
 * PURPOSE:
 *   Represent debugger keymap activation state and command-context precedence.
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
#include "umicom/debug/workbench/debug_keymap_context.h"
#include <string.h>

/*
 * Initialise debug workbench debug keymap context from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_keymap_context_init(UmiDebugWorkbenchDebugKeymapContext *model)
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
 * Provide the debug workbench debug keymap context set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_keymap_context_set_enabled(UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command, bool enabled)
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
 * Provide the debug workbench debug keymap context is enabled operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_debug_keymap_context_is_enabled(const UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command)
{
    uint64_t bit = umi_debug_workbench_command_bit(command);
    return model != NULL && bit != 0U && (model->enabled_commands & bit) != 0U;
}

/*
 * Provide the debug workbench debug keymap context set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_keymap_context_set_primary(UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command)
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
 * Check that debug workbench debug keymap context satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_keymap_context_valid(const UmiDebugWorkbenchDebugKeymapContext *model)
{
    return model != NULL && model->revision > 0U && umi_debug_workbench_command_bit(model->primary_command) != 0U;
}
