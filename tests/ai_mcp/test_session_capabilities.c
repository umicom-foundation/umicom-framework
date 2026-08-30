/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_capabilities.c
 *
 * PURPOSE:
 *   Verify MCP session capabilities behavior.
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
int test_ai_mcp_session_capabilities(void){UmiAiMcpSession s;UmiTestMcpTransportState state;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert((s.capabilities.flags&UMI_AI_MCP_CAPABILITY_TOOLS)!=0U);assert((s.capabilities.flags&UMI_AI_MCP_CAPABILITY_RESOURCES)!=0U);assert((s.capabilities.flags&UMI_AI_MCP_CAPABILITY_PROMPTS)!=0U);return 0;}
