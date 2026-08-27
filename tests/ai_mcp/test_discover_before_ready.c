/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_discover_before_ready.c
 *
 * PURPOSE:
 *   Verify MCP discover before ready behavior through the Framework transport and existing AI runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
int test_ai_mcp_discover_before_ready(void){UmiAiMcpSession s;UmiAiMcpServerDescriptor server;UmiTestMcpTransportState state={0};UmiAiMcpTransport t=umi_test_mcp_transport(&state);UmiAiMcpToolCatalogue c;assert(umi_test_mcp_server(&server)==UMI_STATUS_OK);assert(umi_ai_mcp_session_init(&s,&server,&t)==UMI_STATUS_OK);assert(umi_ai_mcp_discover_tools(&s,&c)==UMI_STATUS_INVALID_STATE);return 0;}
