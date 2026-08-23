/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/cancel.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol cancel request with bounded typed input.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_CANCEL_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_CANCEL_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_cancel(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t request_id,
    uint64_t progress_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
