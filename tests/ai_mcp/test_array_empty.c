/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_array_empty.c
 *
 * PURPOSE:
 *   Verify MCP JSON array empty handling.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai/mcp/json_array.h"
static UmiStatus visit_empty(const char*object,void*user){(void)object;(void)user;return UMI_STATUS_INTERNAL_ERROR;}
int test_ai_mcp_array_empty(void){size_t count=99U;assert(umi_ai_mcp_json_array_visit_objects("{\"tools\":[]}","tools",visit_empty,NULL,&count)==UMI_STATUS_OK);assert(count==0U);return 0;}
