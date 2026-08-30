/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/set_exception_breakpoints.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol setExceptionBreakpoints request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SET_EXCEPTION_BREAKPOINTS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SET_EXCEPTION_BREAKPOINTS_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_set_exception_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const char *const *filters,
    size_t filter_count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
