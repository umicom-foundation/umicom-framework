/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/json.c
 *
 * PURPOSE:
 *   Implement shared JSON argument/result helpers for developer tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/json.h"

#include <string.h>

/*
 * Provide the ai coding tool json parse object operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_parse_object(
    const char *json,
    UmiLanguageRuntimeJsonDocument *out_document)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (json == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_language_runtime_json_parse(json, out_document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_document->token_count == 0U ||
        out_document->tokens[0].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        return UMI_STATUS_PARSE_ERROR;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool json required string operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_required_string(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    char *out_text,
    size_t capacity)
{
    int token;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || key == NULL ||
        out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token < 0) return UMI_STATUS_NOT_FOUND;

    return umi_language_runtime_json_string(
        document, token, out_text, capacity);
}

/*
 * Provide the ai coding tool json optional string operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_optional_string(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    const char *default_value,
    char *out_text,
    size_t capacity)
{
    int token;
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || key == NULL ||
        default_value == NULL || out_text == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (token >= 0) {
        return umi_language_runtime_json_string(
            document, token, out_text, capacity);
    }

    length = strlen(default_value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out_text, default_value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool json optional bool operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_optional_bool(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    int default_value,
    int *out_value)
{
    int token;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token < 0) {
        *out_value = default_value;
        return UMI_STATUS_OK;
    }

    return umi_language_runtime_json_bool(document, token, out_value);
}

/*
 * Provide the ai coding tool json optional uint64 operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_coding_tool_json_optional_uint64(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    uint64_t default_value,
    uint64_t *out_value)
{
    int token;
    int64_t value;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (token < 0) {
        *out_value = default_value;
        return UMI_STATUS_OK;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_language_runtime_json_int64(
            document, token, &value) != UMI_STATUS_OK ||
        value < 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding tool json status operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_json_status(
    char *output,
    size_t capacity,
    UmiStatus status,
    const char *message)
{
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL || capacity == 0U || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, output, capacity);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"status\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, (uint64_t)status);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"ok\":");
    (void)umi_language_runtime_json_writer_bool(
        &writer, status == UMI_STATUS_OK);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"message\":");
    (void)umi_language_runtime_json_writer_string(&writer, message);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    return writer.status;
}
