/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/stack_trace.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol stackTrace request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_STACK_TRACE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_STACK_TRACE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_stack_trace(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint32_t start_frame,
    uint32_t levels,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
