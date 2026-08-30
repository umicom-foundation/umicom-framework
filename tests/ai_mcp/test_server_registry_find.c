/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_find.c
 *
 * PURPOSE:
 *   Verify MCP server registry find behavior.
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
int test_ai_mcp_server_registry_find(void){UmiAiMcpServerRegistry r;UmiAiMcpServerDescriptor s;umi_ai_mcp_server_registry_init(&r);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_find(&r,"test")!=NULL);assert(umi_ai_mcp_server_registry_find(&r,"missing")==NULL);return 0;}
