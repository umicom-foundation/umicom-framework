/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/invocation.c
 *
 * PURPOSE:
 *   Implement MCP tool/resource/prompt requests while keeping approval enforcement in the existing AI runtime and policy path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/invocation.h"

static UmiStatus require_ready(
    const UmiAiMcpSession *session)
{
    if (session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return session->state == UMI_AI_MCP_SESSION_READY
        ? UMI_STATUS_OK
        : UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_ai_mcp_call_tool(
    UmiAiMcpSession *session,
    const char *tool_name,
    const char *arguments_json,
    char *out_result_json,
    size_t result_capacity)
{
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status = require_ready(session);

    if (status != UMI_STATUS_OK) return status;
    if (tool_name == NULL ||
        arguments_json == NULL ||
        out_result_json == NULL ||
        result_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_encode_tool_call_params(
        tool_name,
        arguments_json,
        params,
        sizeof(params));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_mcp_transport_request(
        &session->transport,
        umi_ai_mcp_method_tools_call(),
        params,
        out_result_json,
        result_capacity);
}

UmiStatus umi_ai_mcp_read_resource(
    UmiAiMcpSession *session,
    const char *uri,
    char *out_result_json,
    size_t result_capacity)
{
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status = require_ready(session);

    if (status != UMI_STATUS_OK) return status;
    if (uri == NULL ||
        out_result_json == NULL ||
        result_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_encode_resource_read_params(
        uri,
        params,
        sizeof(params));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_mcp_transport_request(
        &session->transport,
        umi_ai_mcp_method_resources_read(),
        params,
        out_result_json,
        result_capacity);
}

UmiStatus umi_ai_mcp_get_prompt(
    UmiAiMcpSession *session,
    const char *prompt_name,
    const char *arguments_json,
    char *out_result_json,
    size_t result_capacity)
{
    char params[UMI_AI_MCP_TEXT_CAPACITY];
    UmiStatus status = require_ready(session);

    if (status != UMI_STATUS_OK) return status;
    if (prompt_name == NULL ||
        arguments_json == NULL ||
        out_result_json == NULL ||
        result_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_encode_prompt_get_params(
        prompt_name,
        arguments_json,
        params,
        sizeof(params));
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_mcp_transport_request(
        &session->transport,
        umi_ai_mcp_method_prompts_get(),
        params,
        out_result_json,
        result_capacity);
}
