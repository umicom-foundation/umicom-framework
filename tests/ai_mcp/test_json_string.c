/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_string.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON string behavior.
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
 * Exercise test ai mcp json string and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_json_string(void){char value[64];assert(umi_ai_mcp_json_string("{\"name\":\"alpha\"}","name",value,sizeof(value))==UMI_STATUS_OK);assert(strcmp(value,"alpha")==0);return 0;}
