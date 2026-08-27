/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_disconnect.c
 *
 * PURPOSE:
 *   Verify MCP session disconnect behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
int test_ai_mcp_session_disconnect(void){UmiAiMcpSession s;UmiTestMcpTransportState state;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);umi_ai_mcp_session_disconnect(&s);assert(s.state==UMI_AI_MCP_SESSION_DISCONNECTED);assert(s.capabilities.flags==0U);return 0;}
