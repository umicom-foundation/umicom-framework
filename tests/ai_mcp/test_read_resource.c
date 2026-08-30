/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_read_resource.c
 *
 * PURPOSE:
 *   Verify MCP read resource behavior through the Framework transport and existing AI runtime.
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
int test_ai_mcp_read_resource(void){UmiAiMcpSession s;UmiTestMcpTransportState state;char result[512];assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_read_resource(&s,"file:///one",result,sizeof(result))==UMI_STATUS_OK);assert(strcmp(state.last_method,"resources/read")==0);return 0;}
