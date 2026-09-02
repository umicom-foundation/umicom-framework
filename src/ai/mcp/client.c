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

/*
 * Initialise ai mcp client from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ai_mcp_client_init(
    UmiAiMcpClient *client,
    const UmiAiMcpServerDescriptor *server,
    const UmiAiMcpTransport *transport)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the ai mcp client start operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_client_start(
    UmiAiMcpClient *client,
    const char *client_name,
    const char *client_version)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_ai_mcp_session_initialize(
        &client->session,
        client_name,
        client_version);
}

/*
 * Provide the ai mcp client discover operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_client_discover(
    UmiAiMcpClient *client)
{
    UmiStatus status;
    uint32_t flags;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL ||
        client->session.state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    flags = client->session.capabilities.flags;

    /* Apply this branch only when its contract condition is satisfied. */
    if ((flags & UMI_AI_MCP_CAPABILITY_TOOLS) != 0U) {
        status = umi_ai_mcp_discover_tools(
            &client->session,
            &client->tools);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((flags & UMI_AI_MCP_CAPABILITY_RESOURCES) != 0U) {
        status = umi_ai_mcp_discover_resources(
            &client->session,
            &client->resources);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if ((flags & UMI_AI_MCP_CAPABILITY_PROMPTS) != 0U) {
        status = umi_ai_mcp_discover_prompts(
            &client->session,
            &client->prompts);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai mcp client register tools operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_client_register_tools(
    UmiAiMcpClient *client,
    UmiAiToolRegistry *tool_registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (client == NULL || tool_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (client->session.state != UMI_AI_MCP_SESSION_READY) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_ai_mcp_tool_bridge_register(
        &client->tool_bridge,
        &client->session,
        &client->tools,
        tool_registry);
}
