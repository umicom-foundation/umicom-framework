/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_client_register_tools.c
 *
 * PURPOSE:
 *   Verify MCP client register tools behavior through the Framework transport and existing AI runtime.
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
 * Exercise test ai mcp client register tools and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_client_register_tools(void){UmiAiMcpClient c;UmiAiMcpServerDescriptor s;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);UmiAiToolRegistry r;assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_client_init(&c,&s,&t)==UMI_STATUS_OK);assert(umi_ai_mcp_client_start(&c,"client","1")==UMI_STATUS_OK);assert(umi_ai_mcp_client_discover(&c)==UMI_STATUS_OK);umi_ai_tool_registry_init(&r);assert(umi_ai_mcp_client_register_tools(&c,&r)==UMI_STATUS_OK);assert(r.count==2U);return 0;}
