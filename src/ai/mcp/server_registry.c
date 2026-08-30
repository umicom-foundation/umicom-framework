/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/server_registry.c
 *
 * PURPOSE:
 *   Implement deterministic MCP server registration, removal and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/server_registry.h"

#include <string.h>

void umi_ai_mcp_server_registry_init(
    UmiAiMcpServerRegistry *registry)
{
    if (registry == NULL) {
        return;
    }

    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_find(
    UmiAiMcpServerRegistry *registry,
    const char *server_id)
{
    size_t index;

    if (registry == NULL || server_id == NULL) {
        return NULL;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(
                registry->servers[index].server_id,
                server_id) == 0) {
            return &registry->servers[index];
        }
    }

    return NULL;
}

UmiStatus umi_ai_mcp_server_registry_add(
    UmiAiMcpServerRegistry *registry,
    const UmiAiMcpServerDescriptor *server)
{
    UmiStatus status;

    if (registry == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_server_validate(server);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    if (umi_ai_mcp_server_registry_find(
            registry,
            server->server_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (registry->count >= UMI_AI_MCP_MAX_SERVERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->servers[registry->count++] = *server;
    registry->revision += 1U;

    return UMI_STATUS_OK;
}

UmiStatus umi_ai_mcp_server_registry_remove(
    UmiAiMcpServerRegistry *registry,
    const char *server_id)
{
    size_t index;

    if (registry == NULL || server_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(
                registry->servers[index].server_id,
                server_id) == 0) {
            if (index + 1U < registry->count) {
                (void)memmove(
                    &registry->servers[index],
                    &registry->servers[index + 1U],
                    (registry->count - index - 1U) *
                        sizeof(registry->servers[0]));
            }

            registry->count -= 1U;
            registry->revision += 1U;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

const UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_at(
    const UmiAiMcpServerRegistry *registry,
    size_t index)
{
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }

    return &registry->servers[index];
}
