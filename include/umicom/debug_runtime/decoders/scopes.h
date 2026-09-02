/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoders/scopes.h
 *
 * PURPOSE:
 *   Decode DAP scopes response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_SCOPES_H
#define UMICOM_DEBUG_RUNTIME_DECODER_SCOPES_H
#include "umicom/debug_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the debug runtime decode scopes operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decode_scopes(
    const char *json,
    UmiDebugRuntimeScopeList *out_result);
#ifdef __cplusplus
}
#endif
#endif
