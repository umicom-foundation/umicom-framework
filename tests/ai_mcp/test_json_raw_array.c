/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_raw_array.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON raw array behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_raw_array(void){char value[128];assert(umi_ai_mcp_json_raw("{\"items\":[{\"x\":1},{\"x\":2}]}","items",value,sizeof(value))==UMI_STATUS_OK);assert(value[0]=='[');return 0;}
