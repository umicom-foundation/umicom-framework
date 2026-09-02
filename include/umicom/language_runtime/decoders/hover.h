/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/hover.h
 *
 * PURPOSE:
 *   Decode Hover contents and optional range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_HOVER_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_HOVER_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode hover operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_decode_hover(const char*json,UmiLanguageRuntimeHoverResult*out);
#ifdef __cplusplus
}
#endif
#endif
