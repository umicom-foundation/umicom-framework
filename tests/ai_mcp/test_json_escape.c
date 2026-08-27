/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_escape.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON escape behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_escape(void){char value[64];assert(umi_ai_mcp_json_escape_string("a\"b\\c",value,sizeof(value))==UMI_STATUS_OK);assert(strcmp(value,"\"a\\\"b\\\\c\"")==0);return 0;}
