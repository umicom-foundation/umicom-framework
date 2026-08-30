/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_session_snapshot.h
 *
 * PURPOSE:
 *   Capture durable high-level debugger session state for workbench restoration.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_SNAPSHOT_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_SESSION_SNAPSHOT_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugSessionSnapshot {
    char active_session_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    char active_item_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    uint32_t session_count;
    uint32_t breakpoint_count;
    uint32_t thread_count;
    uint32_t watch_count;
    uint64_t generation;
} UmiDebugWorkbenchDebugSessionSnapshot;

void umi_debug_workbench_debug_session_snapshot_init(UmiDebugWorkbenchDebugSessionSnapshot *model);
UmiStatus umi_debug_workbench_debug_session_snapshot_capture(UmiDebugWorkbenchDebugSessionSnapshot *model, const char *session_id, const char *item_id, uint32_t sessions, uint32_t breakpoints, uint32_t threads, uint32_t watches);
int umi_debug_workbench_debug_session_snapshot_valid(const UmiDebugWorkbenchDebugSessionSnapshot *model);

#ifdef __cplusplus
}
#endif
#endif
