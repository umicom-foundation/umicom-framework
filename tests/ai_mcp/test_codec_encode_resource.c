/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_encode_resource.c
 *
 * PURPOSE:
 *   Verify MCP codec encode resource behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/codec.h"
int test_ai_mcp_codec_encode_resource(void){char json[512];assert(umi_ai_mcp_encode_resource_read_params("file:///a",json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"file:///a")!=NULL);return 0;}
