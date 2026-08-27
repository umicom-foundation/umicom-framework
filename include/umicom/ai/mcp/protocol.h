/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/protocol.h
 *
 * PURPOSE:
 *   Expose the stable MCP method names and protocol helpers used by the Framework client.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_PROTOCOL_H
#define UMICOM_AI_MCP_PROTOCOL_H

#include "umicom/ai/mcp/types.h"

#define UMI_AI_MCP_PROTOCOL_VERSION "2025-06-18"

#ifdef __cplusplus
extern "C" {
#endif

const char *umi_ai_mcp_method_initialize(void);
const char *umi_ai_mcp_method_initialized(void);
const char *umi_ai_mcp_method_ping(void);
const char *umi_ai_mcp_method_tools_list(void);
const char *umi_ai_mcp_method_tools_call(void);
const char *umi_ai_mcp_method_resources_list(void);
const char *umi_ai_mcp_method_resources_read(void);
const char *umi_ai_mcp_method_prompts_list(void);
const char *umi_ai_mcp_method_prompts_get(void);

#ifdef __cplusplus
}
#endif
#endif
