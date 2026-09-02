/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_escaped_string.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON escaped string behavior.
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
 * Exercise test ai mcp json escaped string and return a clear result when the behaviour no
 * longer matches its contract.
 */
int test_ai_mcp_json_escaped_string(void){char value[64];assert(umi_ai_mcp_json_string("{\"name\":\"a\\nline\"}","name",value,sizeof(value))==UMI_STATUS_OK);assert(strcmp(value,"a\nline")==0);return 0;}
