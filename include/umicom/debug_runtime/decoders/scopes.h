/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/scopes.h
 *
 * PURPOSE:
 *   Decode DAP scopes response.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_SCOPES_H
#define UMICOM_DEBUG_RUNTIME_DECODER_SCOPES_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_debug_runtime_decode_scopes(
    const char *json,
    UmiDebugRuntimeScopeList *out_result);
#ifdef __cplusplus
}
#endif
#endif
