/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_bridge_register.c
 *
 * PURPOSE:
 *   Verify MCP bridge register behavior through the Framework transport and existing AI runtime.
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
int test_ai_mcp_bridge_register(void){UmiAiMcpSession s;UmiTestMcpTransportState state;UmiAiMcpToolCatalogue c;UmiAiMcpToolBridge bridge;UmiAiToolRegistry tools;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_discover_tools(&s,&c)==UMI_STATUS_OK);umi_ai_tool_registry_init(&tools);assert(umi_ai_mcp_tool_bridge_register(&bridge,&s,&c,&tools)==UMI_STATUS_OK);assert(tools.count==2U);assert(umi_ai_tool_registry_find(&tools,"mcp.test.read_file")!=NULL);return 0;}
