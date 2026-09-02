/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_encode_resource.c
 *
 * PURPOSE:
 *   Verify MCP codec encode resource behavior.
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
 * Exercise test ai mcp codec encode resource and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_codec_encode_resource(void){char json[512];assert(umi_ai_mcp_encode_resource_read_params("file:///a",json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"file:///a")!=NULL);return 0;}
