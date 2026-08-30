/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_initialize.c
 *
 * PURPOSE:
 *   Verify MCP codec initialize behavior.
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
int test_ai_mcp_codec_initialize(void){UmiAiMcpServerCapabilities c;assert(umi_ai_mcp_decode_initialize_result("{\"protocolVersion\":\"2025-06-18\",\"capabilities\":{\"tools\":{},\"resources\":{},\"prompts\":{}},\"serverInfo\":{\"name\":\"srv\",\"version\":\"1\"}}",&c)==UMI_STATUS_OK);assert(strcmp(c.protocol_version,"2025-06-18")==0);assert(strcmp(c.server_name,"srv")==0);assert((c.flags&UMI_AI_MCP_CAPABILITY_TOOLS)!=0U);return 0;}
