/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_tools_list.c
 *
 * PURPOSE:
 *   Verify the MCP tools list method remains the standard protocol string.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/ai/mcp/protocol.h"

int test_ai_mcp_protocol_tools_list(void)
{
    assert(strcmp(umi_ai_mcp_method_tools_list(), "tools/list") == 0);
    return 0;
}
