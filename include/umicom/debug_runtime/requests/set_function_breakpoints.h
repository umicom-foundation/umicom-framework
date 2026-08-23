/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_function_breakpoints.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setFunctionBreakpoints request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_FUNCTION_BREAKPOINTS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_FUNCTION_BREAKPOINTS_H
#include "umicom/debug_runtime/request_support.h"
#include "umicom/debug_runtime/function_breakpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_set_function_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const UmiDebugRuntimeFunctionBreakpoint *breakpoints,
    size_t breakpoint_count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
