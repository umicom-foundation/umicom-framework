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

UmiStatus umi_ai_coding_tool_json_parse_object(
    const char *json,
    UmiLanguageRuntimeJsonDocument *out_document)
{
    UmiStatus status;

    if (json == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_language_runtime_json_parse(json, out_document);
    if (status != UMI_STATUS_OK) return status;

    if (out_document->token_count == 0U ||
        out_document->tokens[0].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        return UMI_STATUS_PARSE_ERROR;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_json_required_string(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    char *out_text,
    size_t capacity)
{
    int token;

    if (document == NULL || key == NULL ||
        out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    if (token < 0) return UMI_STATUS_NOT_FOUND;

    return umi_language_runtime_json_string(
        document, token, out_text, capacity);
}

UmiStatus umi_ai_coding_tool_json_optional_string(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    const char *default_value,
    char *out_text,
    size_t capacity)
{
    int token;
    size_t length;

    if (document == NULL || key == NULL ||
        default_value == NULL || out_text == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    if (token >= 0) {
        return umi_language_runtime_json_string(
            document, token, out_text, capacity);
    }

    length = strlen(default_value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out_text, default_value, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_json_optional_bool(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    int default_value,
    int *out_value)
{
    int token;

    if (document == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    if (token < 0) {
        *out_value = default_value;
        return UMI_STATUS_OK;
    }

    return umi_language_runtime_json_bool(document, token, out_value);
}

UmiStatus umi_ai_coding_tool_json_optional_uint64(
    const UmiLanguageRuntimeJsonDocument *document,
    const char *key,
    uint64_t default_value,
    uint64_t *out_value)
{
    int token;
    int64_t value;

    if (document == NULL || key == NULL || out_value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    token = umi_language_runtime_json_object_get(document, 0, key);
    if (token < 0) {
        *out_value = default_value;
        return UMI_STATUS_OK;
    }

    if (umi_language_runtime_json_int64(
            document, token, &value) != UMI_STATUS_OK ||
        value < 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_json_status(
    char *output,
    size_t capacity,
    UmiStatus status,
    const char *message)
{
    UmiLanguageRuntimeJsonWriter writer;

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
