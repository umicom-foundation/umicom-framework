/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_remove.c
 *
 * PURPOSE:
 *   Verify MCP server registry remove behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
int test_ai_mcp_server_registry_remove(void){UmiAiMcpServerRegistry r;UmiAiMcpServerDescriptor s;umi_ai_mcp_server_registry_init(&r);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_remove(&r,"test")==UMI_STATUS_OK);assert(r.count==0U);return 0;}
