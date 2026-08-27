/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_tool.c
 *
 * PURPOSE:
 *   Verify MCP codec tool behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/codec.h"
int test_ai_mcp_codec_tool(void){UmiAiMcpToolDescriptor t;assert(umi_ai_mcp_decode_tool("{\"name\":\"read\",\"description\":\"Read\",\"inputSchema\":{\"type\":\"object\"}}",&t)==UMI_STATUS_OK);assert(strcmp(t.name,"read")==0);assert(strstr(t.input_schema,"object")!=NULL);return 0;}
