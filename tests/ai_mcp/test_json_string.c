/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_string.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON string behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_string(void){char value[64];assert(umi_ai_mcp_json_string("{\"name\":\"alpha\"}","name",value,sizeof(value))==UMI_STATUS_OK);assert(strcmp(value,"alpha")==0);return 0;}
