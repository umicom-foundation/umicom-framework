/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_session_initialize_twice.c
 *
 * PURPOSE:
 *   Verify MCP session initialize twice behavior.
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
 * Exercise test ai mcp session initialize twice and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_ai_mcp_session_initialize_twice(void){UmiAiMcpSession s;UmiTestMcpTransportState state;assert(umi_test_mcp_ready_session(&s,&state)==UMI_STATUS_OK);assert(umi_ai_mcp_session_initialize(&s,"client","1")==UMI_STATUS_INVALID_STATE);return 0;}
