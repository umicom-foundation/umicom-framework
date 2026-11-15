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
#include <stdlib.h>

/*
 * Initialise ai mcp client from caller-provided values so later operations receive a known
 * state.
 */
/* Stage the small session before clearing the larger client, preserving prior state on validation failure without allocating or relocating a temporary client. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus umi_ai_mcp_client_init(UmiAiMcpClient *client,
    const UmiAiMcpServerDescriptor *server, const UmiAiMcpTransport *transport)
{
    if (client == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiMcpSession session;
    UmiStatus status = umi_ai_mcp_session_init(&session, server, transport);
    if (status != UMI_STATUS_OK) return status;
    /* All fallible work and borrowed-input reads are complete. Publish the fresh
     * client in place so embedded owners keep their final, caller-chosen address. */
    memset(client, 0, sizeof(*client));
    umi_ai_mcp_tool_catalogue_init(&client->tools);
    umi_ai_mcp_resource_catalogue_init(&client->resources);
    umi_ai_mcp_prompt_catalogue_init(&client->prompts);
    client->session = session;
    return UMI_STATUS_OK;
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
/* Publish supported catalogue updates together only after every requested list succeeds, while keeping session and bridge owners at stable addresses. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Keep all negotiated catalogue updates private until every requested list
 * succeeds. The existing session and bridge are never copied or relocated. */
typedef struct McpDiscoveredCatalogues {
    UmiAiMcpToolCatalogue tools;
    UmiAiMcpResourceCatalogue resources;
    UmiAiMcpPromptCatalogue prompts;
} McpDiscoveredCatalogues;

UmiStatus umi_ai_mcp_client_discover(UmiAiMcpClient *client)
{
    if (client == NULL || client->session.state != UMI_AI_MCP_SESSION_READY)
        return UMI_STATUS_INVALID_STATE;
    const uint32_t flags = client->session.capabilities.flags;
    const uint32_t supported = UMI_AI_MCP_CAPABILITY_TOOLS |
        UMI_AI_MCP_CAPABILITY_RESOURCES | UMI_AI_MCP_CAPABILITY_PROMPTS;
    if ((flags & supported) == 0U) return UMI_STATUS_OK;
    const uint64_t revision = client->session.revision;
    McpDiscoveredCatalogues *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    if ((flags & UMI_AI_MCP_CAPABILITY_TOOLS) != 0U)
        status = umi_ai_mcp_discover_tools(&client->session, &candidate->tools);
    if (status == UMI_STATUS_OK && (flags & UMI_AI_MCP_CAPABILITY_RESOURCES) != 0U)
        status = umi_ai_mcp_discover_resources(&client->session, &candidate->resources);
    if (status == UMI_STATUS_OK && (flags & UMI_AI_MCP_CAPABILITY_PROMPTS) != 0U)
        status = umi_ai_mcp_discover_prompts(&client->session, &candidate->prompts);
    if (status == UMI_STATUS_OK && (client->session.state != UMI_AI_MCP_SESSION_READY ||
        client->session.revision != revision || client->session.capabilities.flags != flags))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) {
        /* Unsupported catalogues retain their earlier bytes. No callback runs
         * between these publication writes; hosts still serialize ownership. */
        if ((flags & UMI_AI_MCP_CAPABILITY_TOOLS) != 0U) client->tools = candidate->tools;
        if ((flags & UMI_AI_MCP_CAPABILITY_RESOURCES) != 0U) client->resources = candidate->resources;
        if ((flags & UMI_AI_MCP_CAPABILITY_PROMPTS) != 0U) client->prompts = candidate->prompts;
    }
    free(candidate);
    return status;
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
