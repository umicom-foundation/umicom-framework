/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_call_before_ready.c
 *
 * PURPOSE:
 *   Verify MCP call before ready behavior through the Framework transport and existing AI runtime.
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
int test_ai_mcp_call_before_ready(void){UmiAiMcpSession s={0};char result[64];assert(umi_ai_mcp_call_tool(&s,"x","{}",result,sizeof(result))==UMI_STATUS_INVALID_STATE);return 0;}
