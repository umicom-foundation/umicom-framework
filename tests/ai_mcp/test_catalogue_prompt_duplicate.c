/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_prompt_duplicate.c
 *
 * PURPOSE:
 *   Verify MCP catalogue prompt duplicate behavior.
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
#include "umicom/ai/mcp/prompt_catalogue.h"
int test_ai_mcp_catalogue_prompt_duplicate(void){UmiAiMcpPromptCatalogue c;UmiAiMcpPromptDescriptor d={0};umi_ai_mcp_prompt_catalogue_init(&c);strcpy(d.name,"review");assert(umi_ai_mcp_prompt_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_prompt_catalogue_add(&c,&d)==UMI_STATUS_ALREADY_EXISTS);return 0;}
