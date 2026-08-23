/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoders/initialize.h
 *
 * PURPOSE:
 *   Decode initialize-result server capability flags.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_INITIALIZE_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_INITIALIZE_H
#include "umicom/language_runtime/decoder_support.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_language_runtime_decode_initialize(const char*json,UmiLanguageRuntimeInitializeResult*out);
#ifdef __cplusplus
}
#endif
#endif
