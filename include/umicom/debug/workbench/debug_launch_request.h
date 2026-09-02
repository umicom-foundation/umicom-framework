/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_launch_request.h
 *
 * PURPOSE:
 *   Capture a validated workbench launch request before dispatch to the canonical debug runtime.
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
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_LAUNCH_REQUEST_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_LAUNCH_REQUEST_H

#include "umicom/debug/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug launch request data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugLaunchRequest {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugLaunchRequest;

/**
 * Initialise debug workbench debug launch request from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_launch_request_init(UmiDebugWorkbenchDebugLaunchRequest *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
/**
 * Provide the debug workbench debug launch request set state operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_workbench_debug_launch_request_set_state(UmiDebugWorkbenchDebugLaunchRequest *model, uint32_t state, uint64_t value);
/**
 * Find debug workbench debug launch request set while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_debug_workbench_debug_launch_request_set_selected(UmiDebugWorkbenchDebugLaunchRequest *model, bool selected);
/**
 * Provide the debug workbench debug launch request set enabled operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_workbench_debug_launch_request_set_enabled(UmiDebugWorkbenchDebugLaunchRequest *model, bool enabled);
/**
 * Check that debug workbench debug launch request satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_launch_request_valid(const UmiDebugWorkbenchDebugLaunchRequest *model);

#ifdef __cplusplus
}
#endif
#endif
