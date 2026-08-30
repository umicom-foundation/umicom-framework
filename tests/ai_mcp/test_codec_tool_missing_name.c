/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_tool_missing_name.c
 *
 * PURPOSE:
 *   Verify MCP codec tool missing name behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/codec.h"
int test_ai_mcp_codec_tool_missing_name(void){UmiAiMcpToolDescriptor t;assert(umi_ai_mcp_decode_tool("{\"description\":\"x\"}",&t)==UMI_STATUS_PARSE_ERROR);return 0;}
