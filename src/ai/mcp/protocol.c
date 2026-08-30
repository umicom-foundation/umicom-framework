/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/protocol.c
 *
 * PURPOSE:
 *   Return MCP method names from one Framework-owned implementation so clients do not scatter protocol strings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/protocol.h"

const char *umi_ai_mcp_method_initialize(void) { return "initialize"; }
const char *umi_ai_mcp_method_initialized(void) { return "notifications/initialized"; }
const char *umi_ai_mcp_method_ping(void) { return "ping"; }
const char *umi_ai_mcp_method_tools_list(void) { return "tools/list"; }
const char *umi_ai_mcp_method_tools_call(void) { return "tools/call"; }
const char *umi_ai_mcp_method_resources_list(void) { return "resources/list"; }
const char *umi_ai_mcp_method_resources_read(void) { return "resources/read"; }
const char *umi_ai_mcp_method_prompts_list(void) { return "prompts/list"; }
const char *umi_ai_mcp_method_prompts_get(void) { return "prompts/get"; }
