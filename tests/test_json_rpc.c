/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_json_rpc.c
 *
 * PURPOSE:
 *   Verify JSON-RPC request construction and parsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/protocol/json_rpc.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char json[1024];
    UmiJsonRpcMessage message;
    assert(umi_json_rpc_build_request(42, "initialize", "{\\\"rootUri\\\":\\\"file:///tmp\\\"}", json, sizeof(json)) == UMI_STATUS_OK);
    assert(umi_json_rpc_parse(json, &message) == UMI_STATUS_OK);
    assert(message.kind == UMI_JSON_RPC_REQUEST);
    assert(message.id == 42);
    assert(strcmp(message.method, "initialize") == 0);
    assert(umi_json_rpc_build_error(42, -32601, "missing", json, sizeof(json)) == UMI_STATUS_OK);
    assert(umi_json_rpc_parse(json, &message) == UMI_STATUS_OK);
    assert(message.kind == UMI_JSON_RPC_ERROR);
    return 0;
}
