/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_session_restore.h
 *
 * PURPOSE:
 *   Describe restoration intent and validation state for a saved debug session.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_RESTORE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_RESTORE_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugSessionRestore {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugSessionRestore;

UmiStatus umi_debug_workbench_debug_session_restore_init(UmiDebugWorkbenchDebugSessionRestore *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_session_restore_set_state(UmiDebugWorkbenchDebugSessionRestore *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_session_restore_set_selected(UmiDebugWorkbenchDebugSessionRestore *model, bool selected);
UmiStatus umi_debug_workbench_debug_session_restore_set_enabled(UmiDebugWorkbenchDebugSessionRestore *model, bool enabled);
int umi_debug_workbench_debug_session_restore_valid(const UmiDebugWorkbenchDebugSessionRestore *model);

#ifdef __cplusplus
}
#endif
#endif
