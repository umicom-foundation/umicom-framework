/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/step_in.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol stepIn request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_STEP_IN_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_STEP_IN_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request step in operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_step_in(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
