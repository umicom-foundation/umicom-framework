/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_raw_object.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON raw object behavior.
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
int test_ai_mcp_json_raw_object(void){char value[128];assert(umi_ai_mcp_json_raw("{\"schema\":{\"type\":\"object\",\"nested\":{\"x\":1}}}","schema",value,sizeof(value))==UMI_STATUS_OK);assert(strstr(value,"nested")!=NULL);return 0;}
