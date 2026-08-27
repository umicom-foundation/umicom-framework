/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_prompt_at.c
 *
 * PURPOSE:
 *   Verify MCP catalogue prompt at behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/prompt_catalogue.h"
int test_ai_mcp_catalogue_prompt_at(void){UmiAiMcpPromptCatalogue c;UmiAiMcpPromptDescriptor d={0};umi_ai_mcp_prompt_catalogue_init(&c);strcpy(d.name,"review");assert(umi_ai_mcp_prompt_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_prompt_catalogue_at(&c,0)!=NULL);assert(umi_ai_mcp_prompt_catalogue_at(&c,1)==NULL);return 0;}
