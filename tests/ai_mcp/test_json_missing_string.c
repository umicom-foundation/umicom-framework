/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_missing_string.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON missing string behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_missing_string(void){char value[64];assert(umi_ai_mcp_json_string("{}","name",value,sizeof(value))==UMI_STATUS_NOT_FOUND);return 0;}
