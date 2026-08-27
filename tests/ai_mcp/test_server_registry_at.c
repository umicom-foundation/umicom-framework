/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_at.c
 *
 * PURPOSE:
 *   Verify MCP server registry at behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "test_support.h"
int test_ai_mcp_server_registry_at(void){UmiAiMcpServerRegistry r;UmiAiMcpServerDescriptor s;umi_ai_mcp_server_registry_init(&r);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_at(&r,0)!=NULL);assert(umi_ai_mcp_server_registry_at(&r,1)==NULL);return 0;}
