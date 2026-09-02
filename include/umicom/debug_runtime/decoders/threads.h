/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/threads.h
 *
 * PURPOSE:
 *   Decode DAP threads response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_THREADS_H
#define UMICOM_DEBUG_RUNTIME_DECODER_THREADS_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode threads operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_threads(
    const char *json,
    UmiDebugRuntimeThreadList *out_result);
#ifdef __cplusplus
}
#endif
#endif
