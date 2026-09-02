/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_resources_list.c
 *
 * PURPOSE:
 *   Verify the MCP resources list method remains the standard protocol string.
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

#include "umicom/ai/mcp/protocol.h"

/*
 * Exercise test ai mcp protocol resources list and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_protocol_resources_list(void)
{
    assert(strcmp(umi_ai_mcp_method_resources_list(), "resources/list") == 0);
    return 0;
}
