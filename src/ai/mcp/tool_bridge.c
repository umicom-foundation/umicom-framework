/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/tool_bridge.c
 *
 * PURPOSE:
 *   Implement MCP-to-existing-AI-tool registration without creating a parallel tool execution path.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/tool_bridge.h"

#include <stdio.h>
#include <string.h>

static UmiStatus invoke_remote_tool(
    const char *arguments_json,
    char *output,
    size_t output_capacity,
    void *user_data)
{
    UmiAiMcpToolBinding *binding =
        (UmiAiMcpToolBinding *)user_data;

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

UmiStatus umi_ai_mcp_tool_bridge_register(
    UmiAiMcpToolBridge *bridge,
    UmiAiMcpSession *session,
    const UmiAiMcpToolCatalogue *catalogue,
    UmiAiToolRegistry *tool_registry)
{
    size_t index;

    if (bridge == NULL ||
        session == NULL ||
        catalogue == NULL ||
        tool_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(bridge, 0, sizeof(*bridge));

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

        if (written < 0 ||
            (size_t)written >= sizeof(tool_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        if (umi_ai_tool_registry_find(
                tool_registry,
                tool_id) != NULL) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

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
        if (written < 0 ||
            (size_t)written >= sizeof(tool_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        if (descriptor->permission[0] != '\0') {
            (void)strncpy(
                permission,
                descriptor->permission,
                sizeof(permission) - 1U);
            permission[sizeof(permission) - 1U] = '\0';
        } else {
            written = snprintf(
                permission,
                sizeof(permission),
                "%s%s%s",
                session->server.permission_prefix,
                session->server.permission_prefix[0] != '\0'
                    ? "."
                    : "",
                descriptor->name);
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
        if (status != UMI_STATUS_OK) {
            return status;
        }

        tool.flags = descriptor->flags;

        status = umi_ai_tool_registry_add(
            tool_registry,
            &tool);
        if (status != UMI_STATUS_OK) {
            return status;
        }

        bridge->binding_count += 1U;
    }

    return UMI_STATUS_OK;
}
