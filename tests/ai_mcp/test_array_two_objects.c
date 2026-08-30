/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_array_two_objects.c
 *
 * PURPOSE:
 *   Verify MCP JSON array two objects handling.
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
static UmiStatus visit(const char*object,void*user){size_t*count=(size_t*)user;assert(object[0]=='{');*count+=1U;return UMI_STATUS_OK;}
int test_ai_mcp_array_two_objects(void){size_t count=0,visited=0;assert(umi_ai_mcp_json_array_visit_objects("{\"tools\":[{\"name\":\"a\"},{\"name\":\"b\"}]}","tools",visit,&visited,&count)==UMI_STATUS_OK);assert(count==2U&&visited==2U);return 0;}
