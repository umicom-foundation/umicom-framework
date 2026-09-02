/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/lsp_did_close.c
 *
 * PURPOSE:
 *   Implement the lsp did close behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework LSP didClose. Sammy Hegab, Umicom Foundation, MIT. */
#include "umicom/protocol/lsp_extended.h"
#include <stdio.h>
#include "umicom/protocol/json.h"
/* Provide the lsp did close operation used by this module and its client applications. */
UmiStatus umi_lsp_did_close(UmiLspClient *client, const char *uri)
{
    char escaped[UMI_PROTOCOL_URI_CAPACITY * 2U], params[UMI_PROTOCOL_MESSAGE_CAPACITY];
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL || client->client == NULL || uri == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_json_escape(uri, escaped, sizeof(escaped)) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(params, sizeof(params), "{\"textDocument\":{\"uri\":\"%s\"}}", escaped);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(params)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_protocol_client_notify(client->client, "textDocument/didClose", params);
}
