/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_codec_prompt.c
 *
 * PURPOSE:
 *   Verify MCP codec prompt behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/codec.h"
int test_ai_mcp_codec_prompt(void){UmiAiMcpPromptDescriptor p;assert(umi_ai_mcp_decode_prompt("{\"name\":\"review\",\"description\":\"Review\"}",&p)==UMI_STATUS_OK);assert(strcmp(p.name,"review")==0);return 0;}
