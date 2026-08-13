/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_request_internal.c
 * PURPOSE: Centralise bounded JSON building for extended LSP requests.
 * Created by: Sammy Hegab | Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#include "lsp_request_internal.h"

#include <stdio.h>
#include <string.h>

#include "umicom/protocol/json.h"

UmiStatus umi_lsp_escape_text(const char *text, char *out_text,
                              size_t capacity)
{
    if (text == NULL || out_text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_json_escape(text, out_text, capacity);
}

static UmiStatus send_request(UmiLspClient *client, const char *method,
                              const char *params, int64_t *out_request_id)
{
    if (client == NULL || client->client == NULL || method == NULL ||
        params == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_protocol_client_request(client->client, method, params,
                                       out_request_id);
}

UmiStatus umi_lsp_request_document(UmiLspClient *client, const char *method,
                                   const char *uri,
                                   int64_t *out_request_id)
{
    char escaped[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[UMI_PROTOCOL_URI_CAPACITY * 2U + 64U];
    int written;
    UmiStatus status = umi_lsp_escape_text(uri, escaped, sizeof(escaped));
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(params, sizeof(params),
                       "{\"textDocument\":{\"uri\":\"%s\"}}", escaped);
    if (written < 0 || (size_t)written >= sizeof(params)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return send_request(client, method, params, out_request_id);
}

UmiStatus umi_lsp_request_position(UmiLspClient *client, const char *method,
                                   const char *uri, UmiLspPosition position,
                                   const char *suffix_json,
                                   int64_t *out_request_id)
{
    char escaped[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    const char *suffix = suffix_json != NULL ? suffix_json : "";
    int written;
    UmiStatus status = umi_lsp_escape_text(uri, escaped, sizeof(escaped));
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(params, sizeof(params),
                       "{\"textDocument\":{\"uri\":\"%s\"},"
                       "\"position\":{\"line\":%u,\"character\":%u}%s}",
                       escaped, position.line, position.character, suffix);
    if (written < 0 || (size_t)written >= sizeof(params)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return send_request(client, method, params, out_request_id);
}

UmiStatus umi_lsp_request_range(UmiLspClient *client, const char *method,
                                const char *uri, UmiLspRange range,
                                const char *suffix_json,
                                int64_t *out_request_id)
{
    char escaped[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    const char *suffix = suffix_json != NULL ? suffix_json : "";
    int written;
    UmiStatus status = umi_lsp_escape_text(uri, escaped, sizeof(escaped));
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(params, sizeof(params),
                       "{\"textDocument\":{\"uri\":\"%s\"},"
                       "\"range\":{\"start\":{\"line\":%u,\"character\":%u},"
                       "\"end\":{\"line\":%u,\"character\":%u}}%s}",
                       escaped, range.start.line, range.start.character,
                       range.end.line, range.end.character, suffix);
    if (written < 0 || (size_t)written >= sizeof(params)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return send_request(client, method, params, out_request_id);
}
