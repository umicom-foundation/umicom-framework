/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoder_support.h
 *
 * PURPOSE:
 *   Share bounded JSON/DAP response decoding helpers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_SUPPORT_H
#define UMICOM_DEBUG_RUNTIME_DECODER_SUPPORT_H
#include "umicom/debug_runtime/results.h"
#include "umicom/language_runtime/json.h"
#ifdef __cplusplus
extern "C" {
#endif

int umi_debug_runtime_decoder_body_token(
    const UmiLanguageRuntimeJsonDocument *document);

int umi_debug_runtime_decoder_event_body_token(
    const UmiLanguageRuntimeJsonDocument *document);

UmiStatus umi_debug_runtime_decoder_optional_string(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    char *out_text,
    size_t capacity);

int64_t umi_debug_runtime_decoder_optional_int(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    int64_t default_value);

int umi_debug_runtime_decoder_optional_bool(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    int default_value);

UmiStatus umi_debug_runtime_decoder_source(
    const UmiLanguageRuntimeJsonDocument *document,
    int source_token,
    UmiDebugRuntimeSource *out_source);

#ifdef __cplusplus
}
#endif
#endif
