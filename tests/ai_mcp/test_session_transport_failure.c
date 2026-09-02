/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_transport_failure.c
 *
 * PURPOSE:
 *   Verify MCP session transport failure behavior.
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
 * Exercise test ai mcp session transport failure and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_session_transport_failure(void){UmiAiMcpSession s;UmiAiMcpServerDescriptor server;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);assert(umi_test_mcp_server(&server)==UMI_STATUS_OK);assert(umi_ai_mcp_session_init(&s,&server,&t)==UMI_STATUS_OK);state.forced_status=UMI_STATUS_IO_ERROR;assert(umi_ai_mcp_session_initialize(&s,"client","1")==UMI_STATUS_IO_ERROR);assert(s.state==UMI_AI_MCP_SESSION_FAILED);return 0;}
