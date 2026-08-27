/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_has_key.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON has key behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_has_key(void){assert(umi_ai_mcp_json_has_key("{\"tools\":{}}","tools"));assert(!umi_ai_mcp_json_has_key("{}","tools"));return 0;}
