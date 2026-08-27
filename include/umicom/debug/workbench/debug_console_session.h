/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_console_session.h
 *
 * PURPOSE:
 *   Represent debug-console session state bound to an active debug session.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_SESSION_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_CONSOLE_SESSION_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugConsoleSession {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugConsoleSession;

UmiStatus umi_debug_workbench_debug_console_session_init(UmiDebugWorkbenchDebugConsoleSession *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_console_session_set_state(UmiDebugWorkbenchDebugConsoleSession *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_console_session_set_selected(UmiDebugWorkbenchDebugConsoleSession *model, bool selected);
UmiStatus umi_debug_workbench_debug_console_session_set_enabled(UmiDebugWorkbenchDebugConsoleSession *model, bool enabled);
int umi_debug_workbench_debug_console_session_valid(const UmiDebugWorkbenchDebugConsoleSession *model);

#ifdef __cplusplus
}
#endif
#endif
