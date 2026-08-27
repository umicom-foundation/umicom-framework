/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_resource.c
 *
 * PURPOSE:
 *   Verify MCP codec resource behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/codec.h"
int test_ai_mcp_codec_resource(void){UmiAiMcpResourceDescriptor r;assert(umi_ai_mcp_decode_resource("{\"uri\":\"file:///a\",\"name\":\"A\",\"mimeType\":\"text/plain\"}",&r)==UMI_STATUS_OK);assert(strcmp(r.uri,"file:///a")==0);return 0;}
