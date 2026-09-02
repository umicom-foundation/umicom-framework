/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_discover_tools.c
 *
 * PURPOSE:
 *   Verify MCP discover tools behavior through the Framework transport and existing AI runtime.
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
#include "test_support.h"
/*
 * Exercise test ai mcp discover tools and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_discover_tools(void){UmiAiMcpSession s;UmiTestMcpTransportState state;UmiAiMcpToolCatalogue c;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_discover_tools(&s,&c)==UMI_STATUS_OK);assert(c.count==2U);assert(umi_ai_mcp_tool_catalogue_find(&c,"read_file")!=NULL);return 0;}
