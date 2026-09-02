/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_bridge_policy_denied.c
 *
 * PURPOSE:
 *   Verify MCP bridge policy denied behavior through the Framework transport and existing AI runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
/*
 * Exercise test ai mcp bridge policy denied and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_bridge_policy_denied(void){UmiAiRuntime runtime;UmiAiMcpSession s;UmiTestMcpTransportState state;UmiAiMcpToolCatalogue c;UmiAiMcpToolBridge bridge;char output[512];assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_discover_tools(&s,&c)==UMI_STATUS_OK);umi_ai_runtime_init(&runtime);runtime.policy.allow_tools=0;assert(umi_ai_mcp_tool_bridge_register(&bridge,&s,&c,&runtime.tools)==UMI_STATUS_OK);assert(umi_ai_runtime_invoke_tool(&runtime,"mcp.test.read_file",1,"{}",output,sizeof(output))==UMI_STATUS_PERMISSION_DENIED);return 0;}
