/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/message.c
 *
 * PURPOSE:
 *   Implement DAP request/response construction and bounded envelope parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/message.h"

#include <stdio.h>
#include <string.h>

static UmiStatus parse_uint(
    const UmiLanguageRuntimeJsonDocument *document,
    int object_token,
    const char *key,
    uint64_t *out_value)
{
    int token;
    int64_t value = 0;

    token = umi_language_runtime_json_object_get(
        document, object_token, key);
    if (token < 0) return UMI_STATUS_NOT_FOUND;

    if (umi_language_runtime_json_int64(
            document, token, &value) != UMI_STATUS_OK ||
        value < 0) {
        return UMI_STATUS_PARSE_ERROR;
    }

    *out_value = (uint64_t)value;
    return UMI_STATUS_OK;
}

static UmiStatus optional_string(
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

    return umi_language_runtime_json_string(
        document, token, out_text, capacity);
}

UmiStatus umi_debug_runtime_build_request(
    uint64_t sequence,
    const char *command,
    const char *arguments_json,
    char *out_json,
    size_t capacity)
{
    UmiLanguageRuntimeJsonWriter writer;

    if (sequence == 0U || command == NULL || command[0] == '\0' ||
        out_json == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, out_json, capacity);
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"seq\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, sequence);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"type\":\"request\",\"command\":");
    (void)umi_language_runtime_json_writer_string(&writer, command);

    if (arguments_json != NULL) {
        (void)umi_language_runtime_json_writer_raw(
            &writer, ",\"arguments\":");
        (void)umi_language_runtime_json_writer_raw(
            &writer, arguments_json);
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    return writer.status;
}

UmiStatus umi_debug_runtime_build_response(
    uint64_t sequence,
    uint64_t request_sequence,
    const char *command,
    int success,
    const char *message,
    const char *body_json,
    char *out_json,
    size_t capacity)
{
    UmiLanguageRuntimeJsonWriter writer;

    if (sequence == 0U || request_sequence == 0U ||
        command == NULL || out_json == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, out_json, capacity);
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"seq\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, sequence);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"type\":\"response\",\"request_seq\":");
    (void)umi_language_runtime_json_writer_uint64(
        &writer, request_sequence);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"success\":");
    (void)umi_language_runtime_json_writer_bool(&writer, success);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"command\":");
    (void)umi_language_runtime_json_writer_string(&writer, command);

    if (message != NULL && message[0] != '\0') {
        (void)umi_language_runtime_json_writer_raw(
            &writer, ",\"message\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, message);
    }

    if (body_json != NULL) {
        (void)umi_language_runtime_json_writer_raw(
            &writer, ",\"body\":");
        (void)umi_language_runtime_json_writer_raw(
            &writer, body_json);
    }

    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    return writer.status;
}

UmiStatus umi_debug_runtime_message_parse(
    const char *json,
    UmiDebugRuntimeEnvelope *out_envelope)
{
    UmiLanguageRuntimeJsonDocument document;
    char type[32];
    int success_token;
    UmiStatus status;

    if (json == NULL || out_envelope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_envelope, 0, sizeof(*out_envelope));

    status = umi_language_runtime_json_parse(json, &document);
    if (status != UMI_STATUS_OK) return status;

    if (document.token_count == 0U ||
        document.tokens[0].type != UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        return UMI_STATUS_PARSE_ERROR;
    }

    status = optional_string(
        &document, 0, "type", type, sizeof(type));
    if (status != UMI_STATUS_OK || type[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }

    (void)parse_uint(&document, 0, "seq", &out_envelope->sequence);

    if (strcmp(type, "request") == 0) {
        out_envelope->kind = UMI_DEBUG_RUNTIME_MESSAGE_REQUEST;
        status = optional_string(
            &document, 0, "command",
            out_envelope->command,
            sizeof(out_envelope->command));
    } else if (strcmp(type, "response") == 0) {
        int success = 0;

        out_envelope->kind = UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE;
        status = optional_string(
            &document, 0, "command",
            out_envelope->command,
            sizeof(out_envelope->command));
        if (status == UMI_STATUS_OK) {
            status = parse_uint(
                &document, 0, "request_seq",
                &out_envelope->request_sequence);
        }

        success_token = umi_language_runtime_json_object_get(
            &document, 0, "success");
        if (success_token >= 0 &&
            umi_language_runtime_json_bool(
                &document, success_token, &success) == UMI_STATUS_OK) {
            out_envelope->success = success;
        }

        (void)optional_string(
            &document, 0, "message",
            out_envelope->message,
            sizeof(out_envelope->message));
    } else if (strcmp(type, "event") == 0) {
        out_envelope->kind = UMI_DEBUG_RUNTIME_MESSAGE_EVENT;
        status = optional_string(
            &document, 0, "event",
            out_envelope->event,
            sizeof(out_envelope->event));
    } else {
        return UMI_STATUS_PARSE_ERROR;
    }

    if (status != UMI_STATUS_OK) return status;

    if (strlen(json) >= sizeof(out_envelope->json)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(
        out_envelope->json,
        sizeof(out_envelope->json),
        "%s",
        json);
    return UMI_STATUS_OK;
}

int umi_debug_runtime_message_body_token(
    const UmiLanguageRuntimeJsonDocument *document)
{
    if (document == NULL || document->token_count == 0U) return -1;
    return umi_language_runtime_json_object_get(document, 0, "body");
}
