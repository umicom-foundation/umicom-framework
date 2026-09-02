/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_prompts_list.c
 *
 * PURPOSE:
 *   Verify the MCP prompts list method remains the standard protocol string.
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
 * Exercise test ai mcp protocol prompts list and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_protocol_prompts_list(void)
{
    assert(strcmp(umi_ai_mcp_method_prompts_list(), "prompts/list") == 0);
    return 0;
}
