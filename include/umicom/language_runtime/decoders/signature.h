/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/signature.h
 *
 * PURPOSE:
 *   Decode SignatureHelp active signature/parameter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_SIGNATURE_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_SIGNATURE_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_signature(const char*json,UmiLanguageRuntimeSignatureResult*out);
#ifdef __cplusplus
}
#endif
#endif
