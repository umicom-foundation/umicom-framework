/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_resource.c
 *
 * PURPOSE:
 *   Verify MCP codec resource behavior.
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
 * Exercise test ai mcp codec resource and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_codec_resource(void){UmiAiMcpResourceDescriptor r;assert(umi_ai_mcp_decode_resource("{\"uri\":\"file:///a\",\"name\":\"A\",\"mimeType\":\"text/plain\"}",&r)==UMI_STATUS_OK);assert(strcmp(r.uri,"file:///a")==0);return 0;}
