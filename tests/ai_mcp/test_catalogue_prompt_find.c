/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_prompt_find.c
 *
 * PURPOSE:
 *   Verify MCP catalogue prompt find behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/prompt_catalogue.h"
int test_ai_mcp_catalogue_prompt_find(void){UmiAiMcpPromptCatalogue c;UmiAiMcpPromptDescriptor d={0};umi_ai_mcp_prompt_catalogue_init(&c);strcpy(d.name,"review");assert(umi_ai_mcp_prompt_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_prompt_catalogue_find(&c,"review")!=NULL);assert(umi_ai_mcp_prompt_catalogue_find(&c,"missing")==NULL);return 0;}
