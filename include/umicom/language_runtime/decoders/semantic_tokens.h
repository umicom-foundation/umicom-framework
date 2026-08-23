/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/semantic_tokens.h
 *
 * PURPOSE:
 *   Decode SemanticTokens.data relative integer arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_SEMANTIC_TOKENS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_SEMANTIC_TOKENS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_semantic_tokens(const char*json,UmiLanguageRuntimeSemanticTokens*out);
#ifdef __cplusplus
}
#endif
#endif
