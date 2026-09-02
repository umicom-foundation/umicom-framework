/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/inlay_hints.h
 *
 * PURPOSE:
 *   Decode InlayHint position/label/kind/padding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_INLAY_HINTS_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_INLAY_HINTS_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decode inlay hints operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_inlay_hints(const char*json,UmiLanguageRuntimeInlayHintList*out);
#ifdef __cplusplus
}
#endif
#endif
