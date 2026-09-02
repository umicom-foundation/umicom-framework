/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_encode_initialize.c
 *
 * PURPOSE:
 *   Verify MCP codec encode initialize behavior.
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
 * Initialise test ai mcp codec encode from caller-provided values so later operations
 * receive a known state.
 */
int test_ai_mcp_codec_encode_initialize(void){char json[512];assert(umi_ai_mcp_encode_initialize_params("client","1.0",json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"clientInfo")!=NULL);assert(strstr(json,"client")!=NULL);return 0;}
