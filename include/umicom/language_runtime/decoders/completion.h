/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/completion.h
 *
 * PURPOSE:
 *   Decode CompletionList or CompletionItem arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_COMPLETION_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_COMPLETION_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode completion operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_completion(const char*json,UmiLanguageRuntimeCompletionResult*out);
#ifdef __cplusplus
}
#endif
#endif
