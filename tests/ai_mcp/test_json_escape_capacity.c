/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_escape_capacity.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON escape capacity behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_escape_capacity(void){char value[4];assert(umi_ai_mcp_json_escape_string("long",value,sizeof(value))==UMI_STATUS_CAPACITY_EXCEEDED);return 0;}
