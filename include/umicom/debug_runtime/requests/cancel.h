/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/cancel.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol cancel request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_CANCEL_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_CANCEL_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_cancel(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t request_id,
    uint64_t progress_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
