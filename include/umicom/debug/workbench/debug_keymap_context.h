/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_keymap_context.h
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_KEYMAP_CONTEXT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_KEYMAP_CONTEXT_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugKeymapContext {
    uint64_t enabled_commands;
    uint64_t visible_commands;
    UmiDebugWorkbenchCommand primary_command;
    uint64_t revision;
} UmiDebugWorkbenchDebugKeymapContext;

void umi_debug_workbench_debug_keymap_context_init(UmiDebugWorkbenchDebugKeymapContext *model);
UmiStatus umi_debug_workbench_debug_keymap_context_set_enabled(UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command, bool enabled);
int umi_debug_workbench_debug_keymap_context_is_enabled(const UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command);
UmiStatus umi_debug_workbench_debug_keymap_context_set_primary(UmiDebugWorkbenchDebugKeymapContext *model, UmiDebugWorkbenchCommand command);
int umi_debug_workbench_debug_keymap_context_valid(const UmiDebugWorkbenchDebugKeymapContext *model);

#ifdef __cplusplus
}
#endif
#endif
