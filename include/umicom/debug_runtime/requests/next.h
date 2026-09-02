/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/next.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol next request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_NEXT_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_NEXT_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request next operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_next(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
