/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp.c
 *
 * PURPOSE:
 *   Implement core Language Server Protocol request and notification generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/lsp.h"

#include <stdio.h>
#include <string.h>

#include "umicom/protocol/json.h"

static UmiStatus escape_pair(const char *first,
                             char *escaped_first,
                             size_t first_capacity,
                             const char *second,
                             char *escaped_second,
                             size_t second_capacity)
{
    UmiStatus status = umi_json_escape(first,
                                       escaped_first,
                                       first_capacity);
    if (status == UMI_STATUS_OK) {
        status = umi_json_escape(second,
                                 escaped_second,
                                 second_capacity);
    }
    return status;
}

UmiStatus umi_lsp_client_init(UmiLspClient *client,
                              UmiProtocolClient *protocol_client,
                              const char *root_uri)
{
    if (client == NULL || protocol_client == NULL || root_uri == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(client, 0, sizeof(*client));
    client->client = protocol_client;
    (void)snprintf(client->root_uri,
                   sizeof(client->root_uri),
                   "%s",
                   root_uri);
    return UMI_STATUS_OK;
}

UmiStatus umi_lsp_initialize(UmiLspClient *client,
                             int64_t process_id,
                             int64_t *out_request_id)
{
    char root[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    if (client == NULL || client->client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_json_escape(client->root_uri, root, sizeof(root)) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"processId\":%lld,\"rootUri\":\"%s\","
                   "\"capabilities\":{\"textDocument\":{\"completion\":{"
                   "\"completionItem\":{\"snippetSupport\":false}}}}}",
                   (long long)process_id,
                   root);
    return umi_protocol_client_request(client->client,
                                       "initialize",
                                       params,
                                       out_request_id);
}

UmiStatus umi_lsp_initialized(UmiLspClient *client)
{
    UmiStatus status;
    if (client == NULL || client->client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_protocol_client_notify(client->client,
                                        "initialized",
                                        "{}");
    if (status == UMI_STATUS_OK) {
        client->initialized = 1;
    }
    return status;
}

UmiStatus umi_lsp_did_open(UmiLspClient *client,
                           const char *uri,
                           const char *language_id,
                           int version,
                           const char *text)
{
    char escaped_uri[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char escaped_language[UMI_PROTOCOL_LANGUAGE_CAPACITY * 2U];
    char escaped_text[UMI_PROTOCOL_MESSAGE_CAPACITY / 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    UmiStatus status;
    if (client == NULL || uri == NULL || language_id == NULL ||
        text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = escape_pair(uri,
                         escaped_uri,
                         sizeof(escaped_uri),
                         language_id,
                         escaped_language,
                         sizeof(escaped_language));
    if (status == UMI_STATUS_OK) {
        status = umi_json_escape(text,
                                 escaped_text,
                                 sizeof(escaped_text));
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"textDocument\":{\"uri\":\"%s\","
                   "\"languageId\":\"%s\",\"version\":%d,\"text\":\"%s\"}}",
                   escaped_uri,
                   escaped_language,
                   version,
                   escaped_text);
    return umi_protocol_client_notify(client->client,
                                      "textDocument/didOpen",
                                      params);
}

UmiStatus umi_lsp_did_change(UmiLspClient *client,
                             const char *uri,
                             int version,
                             const char *text)
{
    char escaped_uri[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char escaped_text[UMI_PROTOCOL_MESSAGE_CAPACITY / 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    UmiStatus status;
    if (client == NULL || uri == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_json_escape(uri, escaped_uri, sizeof(escaped_uri));
    if (status == UMI_STATUS_OK) {
        status = umi_json_escape(text,
                                 escaped_text,
                                 sizeof(escaped_text));
    }
    if (status != UMI_STATUS_OK) {
        return status;
    }
    (void)snprintf(params,
                   sizeof(params),
                   "{\"textDocument\":{\"uri\":\"%s\",\"version\":%d},"
                   "\"contentChanges\":[{\"text\":\"%s\"}]}",
                   escaped_uri,
                   version,
                   escaped_text);
    return umi_protocol_client_notify(client->client,
                                      "textDocument/didChange",
                                      params);
}

UmiStatus umi_lsp_completion(UmiLspClient *client,
                             const char *uri,
                             UmiLspPosition position,
                             int64_t *out_request_id)
{
    char escaped_uri[UMI_PROTOCOL_URI_CAPACITY * 2U];
    char params[4096];
    if (client == NULL || uri == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_json_escape(uri, escaped_uri, sizeof(escaped_uri)) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    {
        size_t uri_length = strlen(escaped_uri);
        int written;
        if (uri_length + 96U > sizeof(params) || uri_length > 4000U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        written = snprintf(params,
                           sizeof(params),
                           "{\"textDocument\":{\"uri\":\"%.*s\"},"
                           "\"position\":{\"line\":%u,\"character\":%u}}",
                           (int)uri_length,
                           escaped_uri,
                           position.line,
                           position.character);
        if (written < 0 || (size_t)written >= sizeof(params)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    return umi_protocol_client_request(client->client,
                                       "textDocument/completion",
                                       params,
                                       out_request_id);
}

UmiStatus umi_lsp_shutdown(UmiLspClient *client,
                           int64_t *out_request_id)
{
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_protocol_client_request(client->client,
                                       "shutdown",
                                       "{}",
                                       out_request_id);
}
