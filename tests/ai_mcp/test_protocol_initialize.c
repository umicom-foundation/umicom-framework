/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_initialize.c
 *
 * PURPOSE:
 *   Verify the MCP initialize method remains the standard protocol string.
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
 * Initialise test ai mcp protocol from caller-provided values so later operations receive
 * a known state.
 */
int test_ai_mcp_protocol_initialize(void)
{
    assert(strcmp(umi_ai_mcp_method_initialize(), "initialize") == 0);
    return 0;
}
