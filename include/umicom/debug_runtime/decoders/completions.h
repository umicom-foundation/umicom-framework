/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/completions.h
 *
 * PURPOSE:
 *   Decode DAP completions response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_COMPLETIONS_H
#define UMICOM_DEBUG_RUNTIME_DECODER_COMPLETIONS_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode completions operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_decode_completions(
    const char *json,
    UmiDebugRuntimeCompletionList *out_result);
#ifdef __cplusplus
}
#endif
#endif
