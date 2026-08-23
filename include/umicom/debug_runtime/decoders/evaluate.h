/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/evaluate.h
 *
 * PURPOSE:
 *   Decode DAP evaluate/setVariable/setExpression response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_EVALUATE_H
#define UMICOM_DEBUG_RUNTIME_DECODER_EVALUATE_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_evaluate(
    const char *json,
    UmiDebugRuntimeEvaluateResult *out_result);
#ifdef __cplusplus
}
#endif
#endif
