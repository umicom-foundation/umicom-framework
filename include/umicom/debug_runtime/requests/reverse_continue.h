/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/requests/reverse_continue.h
 *
 * PURPOSE:
 *   Send the Debug Adapter Protocol reverseContinue request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_REQUEST_REVERSE_CONTINUE_H
#define UMICOM_DEBUG_RUNTIME_REQUEST_REVERSE_CONTINUE_H
#include "umicom/debug_runtime/request_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime request reverse continue operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_reverse_continue(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence);
#ifdef __cplusplus
}
#endif
#endif
