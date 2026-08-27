/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_encode_capacity.c
 *
 * PURPOSE:
 *   Verify MCP codec encode capacity behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/codec.h"
int test_ai_mcp_codec_encode_capacity(void){char json[8];assert(umi_ai_mcp_encode_initialize_params("client","1.0",json,sizeof(json))==UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
