/* Umicom Framework LSP didSave. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/protocol/lsp_extended.h"
#include <stdio.h>
#include "umicom/protocol/json.h"
UmiStatus umi_lsp_did_save(UmiLspClient *client, const char *uri, const char *text)
{
    char escaped_uri[UMI_PROTOCOL_URI_CAPACITY * 2U], escaped_text[UMI_PROTOCOL_MESSAGE_CAPACITY / 2U];
    char params[UMI_PROTOCOL_MESSAGE_CAPACITY]; int written; UmiStatus status;
    if (client == NULL || client->client == NULL || uri == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_json_escape(uri, escaped_uri, sizeof(escaped_uri));
    if (status == UMI_STATUS_OK) status = umi_json_escape(text, escaped_text, sizeof(escaped_text));
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(params, sizeof(params), "{\"textDocument\":{\"uri\":\"%s\"},\"text\":\"%s\"}", escaped_uri, escaped_text);
    if (written < 0 || (size_t)written >= sizeof(params)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_protocol_client_notify(client->client, "textDocument/didSave", params);
}
