/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_ping_before_ready.c
 *
 * PURPOSE:
 *   Verify MCP session ping before ready behavior.
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
 * Exercise test ai mcp session ping before ready and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_session_ping_before_ready(void){UmiAiMcpSession s;UmiAiMcpServerDescriptor server;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);assert(umi_test_mcp_server(&server)==UMI_STATUS_OK);assert(umi_ai_mcp_session_init(&s,&server,&t)==UMI_STATUS_OK);assert(umi_ai_mcp_session_ping(&s)==UMI_STATUS_INVALID_STATE);return 0;}
