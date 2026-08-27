/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_call_tool.c
 *
 * PURPOSE:
 *   Verify MCP call tool behavior through the Framework transport and existing AI runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "test_support.h"
int test_ai_mcp_call_tool(void){UmiAiMcpSession s;UmiTestMcpTransportState state;char result[512];assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_call_tool(&s,"read_file","{\"path\":\"a\"}",result,sizeof(result))==UMI_STATUS_OK);assert(strcmp(state.last_method,"tools/call")==0);assert(strstr(state.last_params,"read_file")!=NULL);return 0;}
