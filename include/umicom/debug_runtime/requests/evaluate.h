/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/evaluate.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol evaluate request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_EVALUATE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_EVALUATE_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_evaluate(
    UmiDebugRuntimeAdapter *adapter,
    const char *expression,
    uint64_t frame_id,
    const char *context,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
