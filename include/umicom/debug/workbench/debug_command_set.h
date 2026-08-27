/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_command_set.h
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_COMMAND_SET_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_COMMAND_SET_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugCommandSet {
    uint64_t enabled_commands;
    uint64_t visible_commands;
    UmiDebugWorkbenchCommand primary_command;
    uint64_t revision;
} UmiDebugWorkbenchDebugCommandSet;

void umi_debug_workbench_debug_command_set_init(UmiDebugWorkbenchDebugCommandSet *model);
UmiStatus umi_debug_workbench_debug_command_set_set_enabled(UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command, bool enabled);
int umi_debug_workbench_debug_command_set_is_enabled(const UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command);
UmiStatus umi_debug_workbench_debug_command_set_set_primary(UmiDebugWorkbenchDebugCommandSet *model, UmiDebugWorkbenchCommand command);
int umi_debug_workbench_debug_command_set_valid(const UmiDebugWorkbenchDebugCommandSet *model);

#ifdef __cplusplus
}
#endif
#endif
