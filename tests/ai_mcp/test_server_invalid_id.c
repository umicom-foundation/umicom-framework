/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_invalid_id.c
 *
 * PURPOSE:
 *   Verify MCP server invalid id behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/server.h"
int test_ai_mcp_server_invalid_id(void){UmiAiMcpServerDescriptor s;assert(umi_ai_mcp_server_init(&s,"","Name","stdio://x",UMI_AI_MCP_TRUST_LOCAL)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
