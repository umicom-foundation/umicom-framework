/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_breakpoints.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setBreakpoints request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_BREAKPOINTS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_BREAKPOINTS_H
#include "umicom/debug_runtime/request_support.h"
#include "umicom/debug/breakpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_set_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    const UmiDebugBreakpointSnapshot *breakpoints,
    size_t breakpoint_count,
    int source_modified,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
