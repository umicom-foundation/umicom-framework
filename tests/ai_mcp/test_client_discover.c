/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_client_discover.c
 *
 * PURPOSE:
 *   Verify MCP client discover behavior through the Framework transport and existing AI runtime.
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
 * Exercise test ai mcp client discover and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_client_discover(void){UmiAiMcpClient c;UmiAiMcpServerDescriptor s;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_client_init(&c,&s,&t)==UMI_STATUS_OK);assert(umi_ai_mcp_client_start(&c,"client","1")==UMI_STATUS_OK);assert(umi_ai_mcp_client_discover(&c)==UMI_STATUS_OK);assert(c.tools.count==2U&&c.resources.count==2U&&c.prompts.count==2U);return 0;}
