/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/goto.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol goto request with bounded typed input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_GOTO_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_GOTO_H
#include "umicom/debug_runtime/request_support.h"

#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_request_goto(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t target_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
