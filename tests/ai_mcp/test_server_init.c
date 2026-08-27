/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_mcp/test_server_init.c
 *
 * PURPOSE:
 *   Verify MCP server init behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "test_support.h"
int test_ai_mcp_server_init(void){UmiAiMcpServerDescriptor s;assert(umi_test_mcp_server(&s)==UMI_STATUS_OK);assert(strcmp(s.server_id,"test")==0);assert(s.enabled);return 0;}
