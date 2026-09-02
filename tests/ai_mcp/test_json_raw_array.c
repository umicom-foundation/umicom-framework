/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_raw_array.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON raw array behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/json.h"
/*
 * Exercise test ai mcp json raw array and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_raw_array(void){char value[128];assert(umi_ai_mcp_json_raw("{\"items\":[{\"x\":1},{\"x\":2}]}","items",value,sizeof(value))==UMI_STATUS_OK);assert(value[0]=='[');return 0;}
