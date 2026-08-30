/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_prompts_get.c
 *
 * PURPOSE:
 *   Verify the MCP prompts get method remains the standard protocol string.
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

int test_ai_mcp_protocol_prompts_get(void)
{
    assert(strcmp(umi_ai_mcp_method_prompts_get(), "prompts/get") == 0);
    return 0;
}
