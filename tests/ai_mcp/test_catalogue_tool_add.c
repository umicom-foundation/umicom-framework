/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_catalogue_tool_add.c
 *
 * PURPOSE:
 *   Verify MCP catalogue tool add behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../value_archive/revision_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/tool_catalogue.h"
/*
 * Add test ai mcp catalogue tool only after its inputs and available capacity have been
 * checked.
 */
int test_ai_mcp_catalogue_tool_add(void){UmiAiMcpToolCatalogue c;UmiAiMcpToolDescriptor d={0};umi_ai_mcp_tool_catalogue_init(&c);strcpy(d.name,"read");assert(umi_ai_mcp_tool_catalogue_add(&c,&d)==UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiAiMcpToolCatalogue, &c,
        (umi_ai_mcp_tool_catalogue_add(&c,&d)));
assert(c.count==1U);return 0;}
