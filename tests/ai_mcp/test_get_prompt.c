/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_get_prompt.c
 *
 * PURPOSE:
 *   Verify MCP get prompt behavior through the Framework transport and existing AI runtime.
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
 * Exercise test ai mcp get prompt and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_get_prompt(void){UmiAiMcpSession s;UmiTestMcpTransportState state;char result[512];assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_get_prompt(&s,"review","{}",result,sizeof(result))==UMI_STATUS_OK);assert(strcmp(state.last_method,"prompts/get")==0);return 0;}
