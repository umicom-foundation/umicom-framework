/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/decoder_support.h
 *
 * PURPOSE:
 *   Share bounded JSON/LSP response decoding helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_DECODER_SUPPORT_H
#define UMICOM_LANGUAGE_RUNTIME_DECODER_SUPPORT_H
#include "umicom/language_runtime/json.h"
#include "umicom/language_runtime/results.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the language runtime decoder result token operation used by this module and its
 * client applications.
 */
int umi_language_runtime_decoder_result_token(const UmiLanguageRuntimeJsonDocument*d);
/**
 * Provide the language runtime decoder params token operation used by this module and its
 * client applications.
 */
int umi_language_runtime_decoder_params_token(const UmiLanguageRuntimeJsonDocument*d);
/**
 * Provide the language runtime decoder position operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decoder_position(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimePosition*out);
/**
 * Provide the language runtime decoder range operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_decoder_range(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeRange*out);
/**
 * Provide the language runtime decoder location operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decoder_location(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeLocation*out);
/**
 * Provide the language runtime decoder optional string operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_decoder_optional_string(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,char*out,size_t cap);
/**
 * Provide the language runtime decoder optional bool operation used by this module and its
 * client applications.
 */
int umi_language_runtime_decoder_optional_bool(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int def);
/**
 * Provide the language runtime decoder optional int operation used by this module and its
 * client applications.
 */
int64_t umi_language_runtime_decoder_optional_int(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int64_t def);
#ifdef __cplusplus
}
#endif
#endif
