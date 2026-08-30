/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_workbench_service.h
 *
 * PURPOSE:
 *   Compose canonical debug runtime state into one toolkit-neutral debugger workbench service.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_WORKBENCH_SERVICE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_WORKBENCH_SERVICE_H

#include "umicom/debug/workbench/debug_session_registry.h"
#include "umicom/debug/workbench/breakpoint_collection.h"
#include "umicom/debug/workbench/thread_group.h"
#include "umicom/debug/workbench/watch_expression_collection.h"
#include "umicom/debug/workbench/debug_status_model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDebugWorkbenchDebugWorkbenchService {
    UmiDebugWorkbenchDebugSessionRegistry sessions;
    UmiDebugWorkbenchBreakpointCollection breakpoints;
    UmiDebugWorkbenchThreadGroup threads;
    UmiDebugWorkbenchWatchExpressionCollection watches;
    UmiDebugWorkbenchDebugStatusModel status;
    bool status_initialized;
    uint64_t revision;
} UmiDebugWorkbenchDebugWorkbenchService;

void umi_debug_workbench_debug_workbench_service_init(UmiDebugWorkbenchDebugWorkbenchService *service);
UmiStatus umi_debug_workbench_debug_workbench_service_open_session(UmiDebugWorkbenchDebugWorkbenchService *service, const UmiDebugWorkbenchEntry *session);
UmiStatus umi_debug_workbench_debug_workbench_service_select_session(UmiDebugWorkbenchDebugWorkbenchService *service, const char *session_id);
UmiStatus umi_debug_workbench_debug_workbench_service_refresh_status(UmiDebugWorkbenchDebugWorkbenchService *service, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t frame_count, uint32_t variable_count);
int umi_debug_workbench_debug_workbench_service_valid(const UmiDebugWorkbenchDebugWorkbenchService *service);

#ifdef __cplusplus
}
#endif
#endif
