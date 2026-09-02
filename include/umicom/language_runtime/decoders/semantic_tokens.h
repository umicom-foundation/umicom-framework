/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/semantic_tokens.h
 *
 * PURPOSE:
 *   Decode SemanticTokens.data relative integer arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_SEMANTIC_TOKENS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_SEMANTIC_TOKENS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode semantic tokens operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_decode_semantic_tokens(const char*json,UmiLanguageRuntimeSemanticTokens*out);
#ifdef __cplusplus
}
#endif
#endif
