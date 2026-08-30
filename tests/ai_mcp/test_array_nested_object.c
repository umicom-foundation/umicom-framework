/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_array_nested_object.c
 *
 * PURPOSE:
 *   Verify MCP JSON array nested object handling.
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
#include "umicom/ai/mcp/json_array.h"
static UmiStatus visit_nested(const char*object,void*user){(void)user;assert(strstr(object,"nested")!=NULL);return UMI_STATUS_OK;}
int test_ai_mcp_array_nested_object(void){size_t count=0;assert(umi_ai_mcp_json_array_visit_objects("{\"tools\":[{\"nested\":{\"x\":1}}]}","tools",visit_nested,NULL,&count)==UMI_STATUS_OK);assert(count==1U);return 0;}
