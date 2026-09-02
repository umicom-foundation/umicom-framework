/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_escape.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON escape behavior.
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
 * Exercise test ai mcp json escape and return a clear result when the behaviour no longer
 * matches its contract.
 */
int test_ai_mcp_json_escape(void){char value[64];assert(umi_ai_mcp_json_escape_string("a\"b\\c",value,sizeof(value))==UMI_STATUS_OK);assert(strcmp(value,"\"a\\\"b\\\\c\"")==0);return 0;}
