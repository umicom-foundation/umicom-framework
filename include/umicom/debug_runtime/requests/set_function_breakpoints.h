/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_function_breakpoints.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setFunctionBreakpoints request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_FUNCTION_BREAKPOINTS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_FUNCTION_BREAKPOINTS_H
#include "umicom/debug_runtime/request_support.h"
#include "umicom/debug_runtime/function_breakpoint.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request set function breakpoints operation used by this module
 * and its client applications.
 */
UmiStatus umi_debug_runtime_request_set_function_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const UmiDebugRuntimeFunctionBreakpoint *breakpoints,
    size_t breakpoint_count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
