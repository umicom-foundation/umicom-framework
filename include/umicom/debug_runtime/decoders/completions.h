/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/completions.h
 *
 * PURPOSE:
 *   Decode DAP completions response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_COMPLETIONS_H
#define UMICOM_DEBUG_RUNTIME_DECODER_COMPLETIONS_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_completions(
    const char *json,
    UmiDebugRuntimeCompletionList *out_result);
#ifdef __cplusplus
}
#endif
#endif
