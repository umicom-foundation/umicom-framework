/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_dap_protocol.c
 *
 * PURPOSE:
 *   Verify DAP initialise, launch, breakpoint and disconnect requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include "umicom/protocol/dap.h"
#include "umicom/protocol/dap_breakpoint.h"
#include "umicom/protocol/transport.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProtocolTransport *transport = NULL;
    UmiProtocolClient *client = NULL;
    UmiDapClient dap;
    UmiDapBreakpointRegistry *breakpoints = NULL;
    int64_t request_id = 0;
    UmiDapBreakpoint breakpoint = {0};
    assert(umi_protocol_transport_create_memory(16U, &transport) == UMI_STATUS_OK);
    assert(umi_protocol_client_create(transport, &client) == UMI_STATUS_OK);
    assert(umi_protocol_client_start(client) == UMI_STATUS_OK);
    assert(umi_dap_client_init(&dap, client) == UMI_STATUS_OK);
    assert(umi_dap_initialize(&dap, "cppdbg", &request_id) == UMI_STATUS_OK);
    assert(umi_dap_launch(&dap, "./app", ".", &request_id) == UMI_STATUS_OK);
    assert(umi_dap_breakpoint_registry_create(&breakpoints) == UMI_STATUS_OK);
    (void)snprintf(breakpoint.source_path, sizeof(breakpoint.source_path), "%s", "main.c");
    breakpoint.line = 12;
    breakpoint.enabled = 1;
    assert(umi_dap_breakpoint_add(breakpoints, &breakpoint) == UMI_STATUS_OK);
    assert(umi_dap_breakpoint_build_request(breakpoints, client, "main.c", &request_id) == UMI_STATUS_OK);
    assert(umi_dap_disconnect(&dap, 1, &request_id) == UMI_STATUS_OK);
    umi_dap_breakpoint_registry_destroy(breakpoints);
    umi_protocol_client_destroy(client);
    umi_protocol_transport_destroy(transport);
    return 0;
}
