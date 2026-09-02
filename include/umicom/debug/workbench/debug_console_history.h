/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_console_history.h
 *
 * PURPOSE:
 *   Maintain deterministic debug-console history and active entry selection.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_HISTORY_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_HISTORY_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug console history data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugConsoleHistory {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugConsoleHistory;

/**
 * Initialise debug workbench debug console history from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_console_history_init(UmiDebugWorkbenchDebugConsoleHistory *model);
/**
 * Add debug workbench debug console history only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_debug_workbench_debug_console_history_add(UmiDebugWorkbenchDebugConsoleHistory *model, const UmiDebugWorkbenchEntry *entry);
/**
 * Remove debug workbench debug console history while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_debug_workbench_debug_console_history_remove(UmiDebugWorkbenchDebugConsoleHistory *model, const char *id);
/**
 * Provide the debug workbench debug console history select operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_console_history_select(UmiDebugWorkbenchDebugConsoleHistory *model, const char *id);
/**
 * Find debug workbench debug console history while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_console_history_find(const UmiDebugWorkbenchDebugConsoleHistory *model, const char *id);
/**
 * Check that debug workbench debug console history satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_console_history_valid(const UmiDebugWorkbenchDebugConsoleHistory *model);

#ifdef __cplusplus
}
#endif
#endif
