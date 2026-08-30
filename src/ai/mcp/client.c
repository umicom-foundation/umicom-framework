/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/client.c
 *
 * PURPOSE:
 *   Implement MCP client composition over session, discovery and existing AI tool-registry integration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/client.h"

#include <string.h>

UmiStatus umi_ai_mcp_client_init(
    UmiAiMcpClient *client,
    const UmiAiMcpServerDescriptor *server,
    const UmiAiMcpTransport *transport)
{
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(client, 0, sizeof(*client));
    umi_ai_mcp_tool_catalogue_init(&client->tools);
    umi_ai_mcp_resource_catalogue_init(&client->resources);
    umi_ai_mcp_prompt_catalogue_init(&client->prompts);

    return umi_ai_mcp_session_init(
        &client->session,
        server,
        transport);
}

UmiStatus umi_ai_mcp_client_start(
    UmiAiMcpClient *client,
    const char *client_name,
    const char *client_version)
{
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ai_mcp_session_initialize(
        &client->session,
        client_name,
        client_version);
}

UmiStatus umi_ai_mcp_client_discover(
    UmiAiMcpClient *client)
{
    UmiStatus status;
    uint32_t flags;

    if (client == NULL ||
        client->session.state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    flags = client->session.capabilities.flags;

    if ((flags & UMI_AI_MCP_CAPABILITY_TOOLS) != 0U) {
        status = umi_ai_mcp_discover_tools(
            &client->session,
            &client->tools);
        if (status != UMI_STATUS_OK) return status;
    }

    if ((flags & UMI_AI_MCP_CAPABILITY_RESOURCES) != 0U) {
        status = umi_ai_mcp_discover_resources(
            &client->session,
            &client->resources);
        if (status != UMI_STATUS_OK) return status;
    }

    if ((flags & UMI_AI_MCP_CAPABILITY_PROMPTS) != 0U) {
        status = umi_ai_mcp_discover_prompts(
            &client->session,
            &client->prompts);
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_client_register_tools(
    UmiAiMcpClient *client,
    UmiAiToolRegistry *tool_registry)
{
    if (client == NULL || tool_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (client->session.state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_ai_mcp_tool_bridge_register(
        &client->tool_bridge,
        &client->session,
        &client->tools,
        tool_registry);
}
