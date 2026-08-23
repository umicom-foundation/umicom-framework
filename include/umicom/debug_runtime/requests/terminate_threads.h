/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/terminate_threads.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol terminateThreads request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_TERMINATE_THREADS_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_TERMINATE_THREADS_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_terminate_threads(
    UmiDebugRuntimeAdapter *adapter,
    const uint64_t *thread_ids,
    size_t thread_count,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
