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

/**
 * Represent the debug workbench debug workbench service data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugWorkbenchService {
    UmiDebugWorkbenchDebugSessionRegistry sessions;
    UmiDebugWorkbenchBreakpointCollection breakpoints;
    UmiDebugWorkbenchThreadGroup threads;
    UmiDebugWorkbenchWatchExpressionCollection watches;
    UmiDebugWorkbenchDebugStatusModel status;
    bool status_initialized;
    uint64_t revision;
} UmiDebugWorkbenchDebugWorkbenchService;

/**
 * Initialise debug workbench debug workbench service from caller-provided values so later
 * operations receive a known state.
 */
void umi_debug_workbench_debug_workbench_service_init(UmiDebugWorkbenchDebugWorkbenchService *service);
/**
 * Provide the debug workbench debug workbench service open session operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_open_session(UmiDebugWorkbenchDebugWorkbenchService *service, const UmiDebugWorkbenchEntry *session);
/**
 * Provide the debug workbench debug workbench service select session operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_select_session(UmiDebugWorkbenchDebugWorkbenchService *service, const char *session_id);
/**
 * Provide the debug workbench debug workbench service refresh status operation used by
 * this module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_workbench_service_refresh_status(UmiDebugWorkbenchDebugWorkbenchService *service, UmiDebugWorkbenchSessionPhase phase, const char *stop_reason, uint32_t frame_count, uint32_t variable_count);
/**
 * Check that debug workbench debug workbench service satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workbench_service_valid(const UmiDebugWorkbenchDebugWorkbenchService *service);

#ifdef __cplusplus
}
#endif
#endif
