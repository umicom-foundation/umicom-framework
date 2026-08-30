/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_tool_duplicate.c
 *
 * PURPOSE:
 *   Verify MCP catalogue tool duplicate behavior.
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
#include "umicom/ai/mcp/tool_catalogue.h"
int test_ai_mcp_catalogue_tool_duplicate(void){UmiAiMcpToolCatalogue c;UmiAiMcpToolDescriptor d={0};umi_ai_mcp_tool_catalogue_init(&c);strcpy(d.name,"read");assert(umi_ai_mcp_tool_catalogue_add(&c,&d)==UMI_STATUS_OK);assert(umi_ai_mcp_tool_catalogue_add(&c,&d)==UMI_STATUS_ALREADY_EXISTS);return 0;}
