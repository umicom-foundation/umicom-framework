/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/completion.h
 *
 * PURPOSE:
 *   Decode CompletionList or CompletionItem arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_COMPLETION_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_COMPLETION_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_completion(const char*json,UmiLanguageRuntimeCompletionResult*out);
#ifdef __cplusplus
}
#endif
#endif
