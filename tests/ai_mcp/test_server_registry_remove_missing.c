/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_registry_remove_missing.c
 *
 * PURPOSE:
 *   Verify MCP server registry remove missing behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/server_registry.h"
int test_ai_mcp_server_registry_remove_missing(void){UmiAiMcpServerRegistry r;umi_ai_mcp_server_registry_init(&r);assert(umi_ai_mcp_server_registry_remove(&r,"missing")==UMI_STATUS_NOT_FOUND);return 0;}
