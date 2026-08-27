/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_prompts_list.c
 *
 * PURPOSE:
 *   Verify the MCP prompts list method remains the standard protocol string.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/ai/mcp/protocol.h"

int test_ai_mcp_protocol_prompts_list(void)
{
    assert(strcmp(umi_ai_mcp_method_prompts_list(), "prompts/list") == 0);
    return 0;
}
