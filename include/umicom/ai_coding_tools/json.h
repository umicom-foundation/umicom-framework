/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/json.h
 *
 * PURPOSE:
 *   Provide shared bounded helpers for extracting tool-call arguments and
 *   producing JSON results using the existing Language Runtime JSON engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_JSON_H
#define UMICOM_AI_CODING_TOOLS_JSON_H

#include "umicom/language_runtime/json.h"
#include "umicom/language_runtime/json_writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding tool json parse object operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_parse_object(
    const char *json,
    UmiLanguageRuntimeJsonDocument *out_document);

/**
 * Provide the ai coding tool json required string operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_required_string(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    char *out_text,
    size_t capacity);

/**
 * Provide the ai coding tool json optional string operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_optional_string(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    const char *default_value,
    char *out_text,
    size_t capacity);

/**
 * Provide the ai coding tool json optional bool operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_optional_bool(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    int default_value,
    int *out_value);

/**
 * Provide the ai coding tool json optional uint64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_optional_uint64(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    uint64_t default_value,
    uint64_t *out_value);

/**
 * Provide the ai coding tool json status operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_json_status(
    char *output,
    size_t capacity,
    UmiStatus status,
    const char *message);

#ifdef __cplusplus
}
#endif
#endif
