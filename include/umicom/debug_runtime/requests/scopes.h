/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/scopes.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol scopes request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_SCOPES_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_SCOPES_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_scopes(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t frame_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
