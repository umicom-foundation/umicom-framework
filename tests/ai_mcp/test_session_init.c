/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_init.c
 *
 * PURPOSE:
 *   Verify MCP session init behavior.
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
int test_ai_mcp_session_init(void){UmiAiMcpSession s;UmiAiMcpServerDescriptor server;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);assert(umi_test_mcp_server(&server)==UMI_STATUS_OK);assert(umi_ai_mcp_session_init(&s,&server,&t)==UMI_STATUS_OK);assert(s.state==UMI_AI_MCP_SESSION_CONNECTED);return 0;}
