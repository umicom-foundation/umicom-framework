/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_tool_find.c
 *
 * PURPOSE:
 *   Verify MCP catalogue tool find behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/tool_catalogue.h"
int test_ai_mcp_catalogue_tool_find(void){UmiAiMcpToolCatalogue c;UmiAiMcpToolDescriptor d={0};umi_ai_mcp_tool_catalogue_init(&c);strcpy(d.name,"read");assert(umi_ai_mcp_tool_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_tool_catalogue_find(&c,"read")!=NULL);assert(umi_ai_mcp_tool_catalogue_find(&c,"missing")==NULL);return 0;}
