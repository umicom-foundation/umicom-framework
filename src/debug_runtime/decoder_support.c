/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/decoder_support.c
 *
 * PURPOSE:
 *   Implement common body/optional/source decoding helpers for DAP.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/decoder_support.h"

#include <string.h>

int umi_debug_runtime_decoder_body_token(
    const UmiLanguageRuntimeJsonDocument *document)
{
    if (document == NULL || document->token_count == 0U) return -1;
    return umi_language_runtime_json_object_get(document, 0, "body");
}

int umi_debug_runtime_decoder_event_body_token(
    const UmiLanguageRuntimeJsonDocument *document)
{
    return umi_debug_runtime_decoder_body_token(document);
}

UmiStatus umi_debug_runtime_decoder_optional_string(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    char *out_text,
    size_t capacity)
{
    const int token = umi_language_runtime_json_object_get(
        document, object_token, key);

    if (out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    out_text[0] = '\0';

    if (token < 0 || umi_language_runtime_json_is_null(document, token)) {
        return UMI_STATUS_OK;
    }

    if (document->tokens[token].type == UMI_LANGUAGE_RUNTIME_JSON_STRING) {
        return umi_language_runtime_json_string(
            document, token, out_text, capacity);
    }

    if (document->tokens[token].type ==
        UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE) {
        const UmiLanguageRuntimeJsonToken *item = &document->tokens[token];
        const size_t length = (size_t)(item->end - item->start);

        if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(out_text, document->json + item->start, length);
        out_text[length] = '\0';
        return UMI_STATUS_OK;
    }

    return UMI_STATUS_PARSE_ERROR;
}

int64_t umi_debug_runtime_decoder_optional_int(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    int64_t default_value)
{
    const int token = umi_language_runtime_json_object_get(
        document, object_token, key);
    int64_t value = default_value;

    if (token >= 0) {
        (void)umi_language_runtime_json_int64(document, token, &value);
    }

    return value;
}

int umi_debug_runtime_decoder_optional_bool(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    int default_value)
{
    const int token = umi_language_runtime_json_object_get(
        document, object_token, key);
    int value = default_value;

    if (token >= 0) {
        (void)umi_language_runtime_json_bool(document, token, &value);
    }

    return value;
}

UmiStatus umi_debug_runtime_decoder_source(
    const UmiLanguageRuntimeJsonDocument *document,
    int source_token,
    UmiDebugRuntimeSource *out_source)
{
    int64_t reference;

    if (document == NULL || out_source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_source, 0, sizeof(*out_source));

    if (source_token < 0 ||
        umi_language_runtime_json_is_null(document, source_token)) {
        return UMI_STATUS_OK;
    }

    (void)umi_debug_runtime_decoder_optional_string(
        document, source_token, "name",
        out_source->name, sizeof(out_source->name));
    (void)umi_debug_runtime_decoder_optional_string(
        document, source_token, "path",
        out_source->path, sizeof(out_source->path));

    reference = umi_debug_runtime_decoder_optional_int(
        document, source_token, "sourceReference", 0);
    out_source->source_reference =
        reference > 0 ? (uint64_t)reference : 0U;

    return UMI_STATUS_OK;
}
