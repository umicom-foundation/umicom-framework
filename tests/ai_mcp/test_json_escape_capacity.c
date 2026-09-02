/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_escape_capacity.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON escape capacity behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json.h"
/*
 * Exercise test ai mcp json escape capacity and return a clear result when the behaviour
 * no longer matches its contract.
 */
int test_ai_mcp_json_escape_capacity(void){char value[4];assert(umi_ai_mcp_json_escape_string("long",value,sizeof(value))==UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
