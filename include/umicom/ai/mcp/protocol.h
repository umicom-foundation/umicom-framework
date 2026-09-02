/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/protocol.h
 *
 * PURPOSE:
 *   Expose the stable MCP method names and protocol helpers used by the Framework client.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_PROTOCOL_H
#define UMICOM_AI_MCP_PROTOCOL_H

#include "umicom/ai/mcp/types.h"

#define UMI_AI_MCP_PROTOCOL_VERSION "2025-06-18"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise ai mcp method from caller-provided values so later operations receive a known
 * state.
 */
const char *umi_ai_mcp_method_initialize(void);
/**
 * Provide the ai mcp method initialized operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_initialized(void);
/**
 * Provide the ai mcp method ping operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_ping(void);
/**
 * Provide the ai mcp method tools list operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_tools_list(void);
/**
 * Provide the ai mcp method tools call operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_tools_call(void);
/**
 * Provide the ai mcp method resources list operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_resources_list(void);
/**
 * Read ai mcp method resources into validated module state and return a status when input
 * cannot be used.
 */
const char *umi_ai_mcp_method_resources_read(void);
/**
 * Provide the ai mcp method prompts list operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_prompts_list(void);
/**
 * Provide the ai mcp method prompts get operation used by this module and its client
 * applications.
 */
const char *umi_ai_mcp_method_prompts_get(void);

#ifdef __cplusplus
}
#endif
#endif
