/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/invocation.h
 *
 * PURPOSE:
 *   Invoke MCP tools, read resources and fetch prompts through an initialized MCP session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_INVOCATION_H
#define UMICOM_AI_MCP_INVOCATION_H

#include "umicom/ai/mcp/discovery.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_mcp_call_tool(
    UmiAiMcpSession *session,
    const char *tool_name,
    const char *arguments_json,
    char *out_result_json,
    size_t result_capacity);

UmiStatus umi_ai_mcp_read_resource(
    UmiAiMcpSession *session,
    const char *uri,
    char *out_result_json,
    size_t result_capacity);

UmiStatus umi_ai_mcp_get_prompt(
    UmiAiMcpSession *session,
    const char *prompt_name,
    const char *arguments_json,
    char *out_result_json,
    size_t result_capacity);

#ifdef __cplusplus
}
#endif
#endif
