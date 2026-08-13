/* Umicom Framework LSP didClose. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/protocol/lsp_extended.h"
#include <stdio.h>
#include "umicom/protocol/json.h"
UmiStatus umi_lsp_did_close(UmiLspClient *client, const char *uri)
{
    char escaped[UMI_PROTOCOL_URI_CAPACITY * 2U], params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    int written;
    if (client == NULL || client->client == NULL || uri == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_json_escape(uri, escaped, sizeof(escaped)) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(params, sizeof(params), "{\"textDocument\":{\"uri\":\"%s\"}}", escaped);
    if (written < 0 || (size_t)written >= sizeof(params)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_protocol_client_notify(client->client, "textDocument/didClose", params);
}
