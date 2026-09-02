/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/disconnect.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol disconnect request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_DISCONNECT_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_DISCONNECT_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request disconnect operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_disconnect(
    UmiDebugRuntimeAdapter *adapter,
    int restart,
    int terminate_debuggee,
    int suspend_debuggee,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
