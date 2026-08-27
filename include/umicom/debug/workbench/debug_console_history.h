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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_HISTORY_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_HISTORY_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugConsoleHistory {
    UmiDebugWorkbenchEntry items[UMI_DEBUG_WORKBENCH_MAX_ITEMS];
    size_t count;
    size_t active_index;
    uint64_t revision;
} UmiDebugWorkbenchDebugConsoleHistory;

void umi_debug_workbench_debug_console_history_init(UmiDebugWorkbenchDebugConsoleHistory *model);
UmiStatus umi_debug_workbench_debug_console_history_add(UmiDebugWorkbenchDebugConsoleHistory *model, const UmiDebugWorkbenchEntry *entry);
UmiStatus umi_debug_workbench_debug_console_history_remove(UmiDebugWorkbenchDebugConsoleHistory *model, const char *id);
UmiStatus umi_debug_workbench_debug_console_history_select(UmiDebugWorkbenchDebugConsoleHistory *model, const char *id);
const UmiDebugWorkbenchEntry *umi_debug_workbench_debug_console_history_find(const UmiDebugWorkbenchDebugConsoleHistory *model, const char *id);
int umi_debug_workbench_debug_console_history_valid(const UmiDebugWorkbenchDebugConsoleHistory *model);

#ifdef __cplusplus
}
#endif
#endif
