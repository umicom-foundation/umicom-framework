/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_encode_prompt.c
 *
 * PURPOSE:
 *   Verify MCP codec encode prompt behavior.
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
 * Exercise test ai mcp codec encode prompt and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_codec_encode_prompt(void){char json[512];assert(umi_ai_mcp_encode_prompt_get_params("review","{}",json,sizeof(json))==UMI_STATUS_OK);assert(strstr(json,"review")!=NULL);return 0;}
