/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_tools_call.c
 *
 * PURPOSE:
 *   Verify the MCP tools call method remains the standard protocol string.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/ai/mcp/protocol.h"

int test_ai_mcp_protocol_tools_call(void)
{
    assert(strcmp(umi_ai_mcp_method_tools_call(), "tools/call") == 0);
    return 0;
}
