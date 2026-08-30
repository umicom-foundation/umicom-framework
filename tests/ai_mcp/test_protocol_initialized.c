/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_initialized.c
 *
 * PURPOSE:
 *   Verify the MCP initialized method remains the standard protocol string.
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

int test_ai_mcp_protocol_initialized(void)
{
    assert(strcmp(umi_ai_mcp_method_initialized(), "notifications/initialized") == 0);
    return 0;
}
