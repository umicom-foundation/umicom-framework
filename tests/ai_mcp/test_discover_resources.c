/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_discover_resources.c
 *
 * PURPOSE:
 *   Verify MCP discover resources behavior through the Framework transport and existing AI runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
int test_ai_mcp_discover_resources(void){UmiAiMcpSession s;UmiTestMcpTransportState state;UmiAiMcpResourceCatalogue c;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_discover_resources(&s,&c)==UMI_STATUS_OK);assert(c.count==2U);assert(umi_ai_mcp_resource_catalogue_find(&c,"file:///one")!=NULL);return 0;}
