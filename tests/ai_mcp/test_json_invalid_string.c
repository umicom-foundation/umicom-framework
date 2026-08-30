/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_json_invalid_string.c
 *
 * PURPOSE:
 *   Verify focused MCP JSON invalid string behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json.h"
int test_ai_mcp_json_invalid_string(void){char value[64];assert(umi_ai_mcp_json_string("{\"name\":123}","name",value,sizeof(value))==UMI_STATUS_PARSE_ERROR);return 0;}
