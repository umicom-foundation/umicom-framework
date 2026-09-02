/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_invalid_trust.c
 *
 * PURPOSE:
 *   Verify MCP server invalid trust behavior.
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
/*
 * Exercise test ai mcp server invalid trust and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_server_invalid_trust(void){UmiAiMcpServerDescriptor s;assert(umi_ai_mcp_server_init(&s,"id","Name","stdio://x",(UmiAiMcpTrust)99)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
