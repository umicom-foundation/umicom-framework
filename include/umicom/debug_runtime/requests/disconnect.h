/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/disconnect.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol disconnect request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_DISCONNECT_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_DISCONNECT_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
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
