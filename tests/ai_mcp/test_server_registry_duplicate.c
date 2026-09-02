/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_duplicate.c
 *
 * PURPOSE:
 *   Verify MCP server registry duplicate behavior.
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
 * Exercise test ai mcp server registry duplicate and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_server_registry_duplicate(void){UmiAiMcpServerRegistry r;UmiAiMcpServerDescriptor s;umi_ai_mcp_server_registry_init(&r);assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_OK);assert(umi_ai_mcp_server_registry_add(&r,&s)==UMI_STATUS_ALREADY_EXISTS);return 0;}
