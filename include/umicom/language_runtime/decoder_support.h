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
int umi_language_runtime_decoder_result_token(const UmiLanguageRuntimeJsonDocument*d);
int umi_language_runtime_decoder_params_token(const UmiLanguageRuntimeJsonDocument*d);
UmiStatus umi_language_runtime_decoder_position(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimePosition*out);
UmiStatus umi_language_runtime_decoder_range(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeRange*out);
UmiStatus umi_language_runtime_decoder_location(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeLocation*out);
UmiStatus umi_language_runtime_decoder_optional_string(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,char*out,size_t cap);
int umi_language_runtime_decoder_optional_bool(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int def);
int64_t umi_language_runtime_decoder_optional_int(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int64_t def);
#ifdef __cplusplus
}
#endif
#endif
