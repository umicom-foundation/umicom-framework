/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_protocol_resources_read.c
 *
 * PURPOSE:
 *   Verify the MCP resources read method remains the standard protocol string.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/ai/mcp/protocol.h"

int test_ai_mcp_protocol_resources_read(void)
{
    assert(strcmp(umi_ai_mcp_method_resources_read(), "resources/read") == 0);
    return 0;
}
