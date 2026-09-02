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

/*
 * Initialise ai mcp method from caller-provided values so later operations receive a known
 * state.
 */
const char *umi_ai_mcp_method_initialize(void) { return "initialize"; }
/*
 * Provide the ai mcp method initialized operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_initialized(void) { return "notifications/initialized"; }
/*
 * Provide the ai mcp method ping operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_ping(void) { return "ping"; }
/*
 * Provide the ai mcp method tools list operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_tools_list(void) { return "tools/list"; }
/*
 * Provide the ai mcp method tools call operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_tools_call(void) { return "tools/call"; }
/*
 * Provide the ai mcp method resources list operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_resources_list(void) { return "resources/list"; }
/*
 * Read ai mcp method resources into validated module state and return a status when input
 * cannot be used.
 */
const char *umi_ai_mcp_method_resources_read(void) { return "resources/read"; }
/*
 * Provide the ai mcp method prompts list operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_prompts_list(void) { return "prompts/list"; }
/*
 * Provide the ai mcp method prompts get operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_prompts_get(void) { return "prompts/get"; }
