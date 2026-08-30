/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_client_start.c
 *
 * PURPOSE:
 *   Verify MCP client start behavior through the Framework transport and existing AI runtime.
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
int test_ai_mcp_client_start(void){UmiAiMcpClient c;UmiAiMcpServerDescriptor s;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_client_init(&c,&s,&t)==UMI_STATUS_OK);assert(umi_ai_mcp_client_start(&c,"client","1")==UMI_STATUS_OK);assert(c.session.state==UMI_AI_MCP_SESSION_READY);return 0;}
