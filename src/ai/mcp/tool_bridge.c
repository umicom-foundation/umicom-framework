/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/tool_bridge.c
 *
 * PURPOSE:
 *   Implement MCP-to-existing-AI-tool registration without creating a parallel tool execution path.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/tool_bridge.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the invoke remote tool operation used by this module and its client
 * applications.
 */
static UmiStatus invoke_remote_tool(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiMcpToolBinding *binding =
        (UmiAiMcpToolBinding *)user_data;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (binding == NULL ||
        binding->session == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    return umi_ai_mcp_call_tool(
        binding->session,
        binding->remote_name,
        arguments_json,
        output,
        output_capacity);
}

/* Add ai mcp tool bridge only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_mcp_tool_bridge_register(
    UmiAiMcpToolBridge *bridge,
    UmiAiMcpSession *session,
    const UmiAiMcpToolCatalogue *catalogue,
    UmiAiToolRegistry *tool_registry)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bridge == NULL ||
        session == NULL ||
        catalogue == NULL ||
        tool_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(bridge, 0, sizeof(*bridge));

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count > UMI_AI_MCP_MAX_TOOL_BINDINGS ||
        catalogue->count > UMI_AI_MAX_TOOLS - tool_registry->count) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * Preflight stable local ids before mutating the existing tool registry.
     * A duplicate or overlong id therefore fails without partial registration.
     */
    for (index = 0U; index < catalogue->count; ++index) {
        char tool_id[UMI_AI_ID_CAPACITY];
        int written = snprintf(
            tool_id,
            sizeof(tool_id),
            "mcp.%s.%s",
            session->server.server_id,
            catalogue->items[index].name);

        /* Apply this branch only when its contract condition is satisfied. */
        if (written < 0 ||
            (size_t)written >= sizeof(tool_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_ai_tool_registry_find(
                tool_registry,
                tool_id) != NULL) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        const UmiAiMcpToolDescriptor *descriptor =
            &catalogue->items[index];
        UmiAiMcpToolBinding *binding =
            &bridge->bindings[bridge->binding_count];
        UmiAiTool tool;
        char tool_id[UMI_AI_ID_CAPACITY];
        char permission[UMI_AI_ID_CAPACITY];
        int written;
        UmiStatus status;

        written = snprintf(
            tool_id,
            sizeof(tool_id),
            "mcp.%s.%s",
            session->server.server_id,
            descriptor->name);
        /* Apply this branch only when its contract condition is satisfied. */
        if (written < 0 ||
            (size_t)written >= sizeof(tool_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        /* Apply this branch only when its contract condition is satisfied. */
        if (descriptor->permission[0] != '\0') {
            (void)strncpy(
                permission,
                descriptor->permission,
                sizeof(permission) - 1U);
            permission[sizeof(permission) - 1U] = '\0';
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            written = snprintf(
                permission,
                sizeof(permission),
                "%s%s%s",
                session->server.permission_prefix,
                session->server.permission_prefix[0] != '\0'
                    ? "."
                    : "",
                descriptor->name);
            /* Apply this branch only when its contract condition is satisfied. */
            if (written < 0 ||
                (size_t)written >= sizeof(permission)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }

        binding->session = session;
        (void)strncpy(
            binding->remote_name,
            descriptor->name,
            sizeof(binding->remote_name) - 1U);

        status = umi_ai_tool_init(
            &tool,
            tool_id,
            descriptor->description,
            permission,
            invoke_remote_tool,
            binding);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        tool.flags = descriptor->flags;

        status = umi_ai_tool_registry_add(
            tool_registry,
            &tool);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }

        bridge->binding_count += 1U;
    }

    return UMI_STATUS_OK;
}
