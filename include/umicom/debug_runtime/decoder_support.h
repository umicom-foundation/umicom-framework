/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug_runtime/decoder_support.h
 *
 * PURPOSE:
 *   Share bounded JSON/DAP response decoding helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_RUNTIME_DECODER_SUPPORT_H
#define UMICOM_DEBUG_RUNTIME_DECODER_SUPPORT_H
#include "umicom/debug_runtime/results.h"
#include "umicom/language_runtime/json.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the debug runtime decoder body token operation used by this module and its
 * client applications.
 */
int umi_debug_runtime_decoder_body_token(
    const UmiLanguageRuntimeJsonDocument *document);

/**
 * Provide the debug runtime decoder event body token operation used by this module and its
 * client applications.
 */
int umi_debug_runtime_decoder_event_body_token(
    const UmiLanguageRuntimeJsonDocument *document);

/**
 * Provide the debug runtime decoder optional string operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_decoder_optional_string(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    char *out_text,
    size_t capacity);

/**
 * Provide the debug runtime decoder optional int operation used by this module and its
 * client applications.
 */
int64_t umi_debug_runtime_decoder_optional_int(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    int64_t default_value);

/**
 * Provide the debug runtime decoder optional bool operation used by this module and its
 * client applications.
 */
int umi_debug_runtime_decoder_optional_bool(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    int default_value);

/**
 * Provide the debug runtime decoder source operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_decoder_source(
    const UmiLanguageRuntimeJsonDocument *document,
    int source_token,
    UmiDebugRuntimeSource *out_source);

#ifdef __cplusplus
}
#endif
#endif
