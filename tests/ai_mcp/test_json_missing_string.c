/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_missing_string.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON missing string behavior.
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
 * Exercise test ai mcp json missing string and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_missing_string(void){char value[64];assert(umi_ai_mcp_json_string("{}","name",value,sizeof(value))==UMI_STATUS_NOT_FOUND);return 0;}
