/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/pause.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol pause request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_PAUSE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_PAUSE_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request pause operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_pause(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
