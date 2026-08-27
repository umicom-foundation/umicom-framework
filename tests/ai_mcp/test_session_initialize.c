/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_initialize.c
 *
 * PURPOSE:
 *   Verify MCP session initialize behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "test_support.h"
int test_ai_mcp_session_initialize(void){UmiAiMcpSession s;UmiTestMcpTransportState state;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(s.state==UMI_AI_MCP_SESSION_READY);assert(strcmp(s.capabilities.protocol_version,"2025-06-18")==0);assert(state.notify_count==1U);return 0;}
