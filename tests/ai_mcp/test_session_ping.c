/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_ping.c
 *
 * PURPOSE:
 *   Verify MCP session ping behavior.
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
 * Exercise test ai mcp session ping and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_session_ping(void){UmiAiMcpSession s;UmiTestMcpTransportState state;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_session_ping(&s)==UMI_STATUS_OK);assert(strcmp(state.last_method,"ping")==0);return 0;}
