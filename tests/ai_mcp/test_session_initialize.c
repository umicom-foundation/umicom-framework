/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_initialize.c
 *
 * PURPOSE:
 *   Verify MCP session initialize behavior.
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
 * Initialise test ai mcp session from caller-provided values so later operations receive a
 * known state.
 */
int test_ai_mcp_session_initialize(void){UmiAiMcpSession s;UmiTestMcpTransportState state;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(s.state==UMI_AI_MCP_SESSION_READY);assert(strcmp(s.capabilities.protocol_version,"2025-06-18")==0);assert(state.notify_count==1U);return 0;}
