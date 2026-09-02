/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_add.c
 *
 * PURPOSE:
 *   Verify MCP server registry add behavior.
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
/*
 * Add test ai mcp server registry only after its inputs and available capacity have been
 * checked.
 */
int test_ai_mcp_server_registry_add(void){UmiAiMcpServerRegistry r;UmiAiMcpServerDescriptor s;umi_ai_mcp_server_registry_init(&r);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_OK);assert(r.count==1U);return 0;}
