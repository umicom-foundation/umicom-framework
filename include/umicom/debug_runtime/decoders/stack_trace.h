/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/stack_trace.h
 *
 * PURPOSE:
 *   Decode DAP stackTrace response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_STACK_TRACE_H
#define UMICOM_DEBUG_RUNTIME_DECODER_STACK_TRACE_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode stack trace operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_decode_stack_trace(
    const char *json,
    UmiDebugRuntimeStackTrace *out_result);
#ifdef __cplusplus
}
#endif
#endif
