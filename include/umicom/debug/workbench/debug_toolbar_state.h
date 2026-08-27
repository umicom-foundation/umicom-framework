/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_toolbar_state.h
 *
 * PURPOSE:
 *   Model debugger toolbar command availability independently of any GUI toolkit.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_TOOLBAR_STATE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_TOOLBAR_STATE_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugToolbarState {
    uint64_t enabled_commands;
    uint64_t visible_commands;
    UmiDebugWorkbenchCommand primary_command;
    uint64_t revision;
} UmiDebugWorkbenchDebugToolbarState;

void umi_debug_workbench_debug_toolbar_state_init(UmiDebugWorkbenchDebugToolbarState *model);
UmiStatus umi_debug_workbench_debug_toolbar_state_set_enabled(UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command, bool enabled);
int umi_debug_workbench_debug_toolbar_state_is_enabled(const UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command);
UmiStatus umi_debug_workbench_debug_toolbar_state_set_primary(UmiDebugWorkbenchDebugToolbarState *model, UmiDebugWorkbenchCommand command);
int umi_debug_workbench_debug_toolbar_state_valid(const UmiDebugWorkbenchDebugToolbarState *model);

#ifdef __cplusplus
}
#endif
#endif
