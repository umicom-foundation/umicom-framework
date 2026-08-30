/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_array_malformed.c
 *
 * PURPOSE:
 *   Verify MCP JSON array malformed handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json_array.h"
static UmiStatus visit_bad(const char*object,void*user){(void)object;(void)user;return UMI_STATUS_OK;}
int test_ai_mcp_array_malformed(void){size_t count=0;assert(umi_ai_mcp_json_array_visit_objects("{\"tools\":[1]}","tools",visit_bad,NULL,&count)==UMI_STATUS_PARSE_ERROR);return 0;}
