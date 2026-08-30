/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_lsp_protocol.c
 *
 * PURPOSE:
 *   Verify LSP initialise, document and completion message generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/protocol/lsp.h"
#include "umicom/protocol/transport.h"

int main(void)
{
    UmiProtocolTransport *transport = NULL;
    UmiProtocolClient *client = NULL;
    UmiLspClient lsp;
    UmiLspPosition position = {3U, 5U};
    int64_t request_id = 0;
    assert(umi_protocol_transport_create_memory(16U, &transport) == UMI_STATUS_OK);
    assert(umi_protocol_client_create(transport, &client) == UMI_STATUS_OK);
    assert(umi_protocol_client_start(client) == UMI_STATUS_OK);
    assert(umi_lsp_client_init(&lsp, client, "file:///workspace") == UMI_STATUS_OK);
    assert(umi_lsp_initialize(&lsp, 123, &request_id) == UMI_STATUS_OK);
    assert(request_id > 0);
    assert(umi_lsp_initialized(&lsp) == UMI_STATUS_OK);
    assert(umi_lsp_did_open(&lsp, "file:///workspace/main.c", "c", 1, "int main(void){}") == UMI_STATUS_OK);
    assert(umi_lsp_completion(&lsp, "file:///workspace/main.c", position, &request_id) == UMI_STATUS_OK);
    assert(umi_protocol_transport_count(transport) >= 3U);
    umi_protocol_client_destroy(client);
    umi_protocol_transport_destroy(transport);
    return 0;
}
