/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_array_missing.c
 *
 * PURPOSE:
 *   Verify MCP JSON array missing handling.
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
static UmiStatus visit_missing(const char*object,void*user){(void)object;(void)user;return UMI_STATUS_OK;}
int test_ai_mcp_array_missing(void){size_t count=0;assert(umi_ai_mcp_json_array_visit_objects("{}","tools",visit_missing,NULL,&count)==UMI_STATUS_NOT_FOUND);return 0;}
