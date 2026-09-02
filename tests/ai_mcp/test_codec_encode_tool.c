/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_encode_tool.c
 *
 * PURPOSE:
 *   Verify MCP codec encode tool behavior.
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
#include "umicom/ai/mcp/codec.h"
/*
 * Exercise test ai mcp codec encode tool and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_codec_encode_tool(void){char json[512];assert(umi_ai_mcp_encode_tool_call_params("read","{\"path\":\"a\"}",json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"\"name\":\"read\"")!=NULL);assert(strstr(json,"arguments")!=NULL);return 0;}
