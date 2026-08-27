/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/workbench/debug_command_set.c
 *
 * PURPOSE:
 *   Expose context-sensitive debugger commands for Studio and other thin frontends.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/workbench/debug_command_set.h"
#include <string.h>

void umi_debug_workbench_debug_command_set_init(UmiDebugWorkbenchDebugCommandSet *model)
{
    if (model == NULL) return;
    memset(model, 0, sizeof *model);
    model->primary_command = UMI_DEBUG_WORKBENCH_COMMAND_START;
    model->visible_commands = (umi_debug_workbench_command_bit(UMI_DEBUG_WORKBENCH_COMMAND_COUNT - 1) << 1U) - 1U;
    model->revision = 1U;
}

UmiStatus umi_debug_workbench_debug_command_set_set_enabled(UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command, bool enabled)
{
    uint64_t bit;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    bit = umi_debug_workbench_command_bit(command);
    if (bit == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (enabled) model->enabled_commands |= bit; else model->enabled_commands &= ~bit;
    model->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_debug_command_set_is_enabled(const UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command)
{
    uint64_t bit = umi_debug_workbench_command_bit(command);
    return model != NULL && bit != 0U && (model->enabled_commands & bit) != 0U;
}

UmiStatus umi_debug_workbench_debug_command_set_set_primary(UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command)
{
    if (model == NULL || umi_debug_workbench_command_bit(command) == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    model->primary_command = command;
    model->revision++;
    return UMI_STATUS_OK;
}

int umi_debug_workbench_debug_command_set_valid(const UmiDebugWorkbenchDebugCommandSet *model)
{
    return model != NULL && model->revision > 0U && umi_debug_workbench_command_bit(model->primary_command) != 0U;
}
