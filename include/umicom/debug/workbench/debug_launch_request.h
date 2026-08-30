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

typedef struct UmiDebugWorkbenchDebugLaunchRequest {
    UmiDebugWorkbenchEntry value;
    bool selected;
    bool enabled;
    uint64_t revision;
} UmiDebugWorkbenchDebugLaunchRequest;

UmiStatus umi_debug_workbench_debug_launch_request_init(UmiDebugWorkbenchDebugLaunchRequest *model, const char *id, const char *label, const char *detail, const char *path, UmiDebugWorkbenchRange range);
UmiStatus umi_debug_workbench_debug_launch_request_set_state(UmiDebugWorkbenchDebugLaunchRequest *model, uint32_t state, uint64_t value);
UmiStatus umi_debug_workbench_debug_launch_request_set_selected(UmiDebugWorkbenchDebugLaunchRequest *model, bool selected);
UmiStatus umi_debug_workbench_debug_launch_request_set_enabled(UmiDebugWorkbenchDebugLaunchRequest *model, bool enabled);
int umi_debug_workbench_debug_launch_request_valid(const UmiDebugWorkbenchDebugLaunchRequest *model);

#ifdef __cplusplus
}
#endif
#endif
