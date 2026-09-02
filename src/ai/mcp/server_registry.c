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

/*
 * Initialise ai mcp server registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_mcp_server_registry_init(
    UmiAiMcpServerRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return;
    }

    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

/*
 * Find ai mcp server registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_find(
    UmiAiMcpServerRegistry *registry,
    const char *server_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || server_id == NULL) {
        return NULL;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->servers[index].server_id,
                server_id) == 0) {
            return &registry->servers[index];
        }
    }

    return NULL;
}

/*
 * Add ai mcp server registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_mcp_server_registry_add(
    UmiAiMcpServerRegistry *registry,
    const UmiAiMcpServerDescriptor *server)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_mcp_server_validate(server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_mcp_server_registry_find(
            registry,
            server->server_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_AI_MCP_MAX_SERVERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->servers[registry->count++] = *server;
    registry->revision += 1U;

    return UMI_STATUS_OK;
}

/*
 * Remove ai mcp server registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_mcp_server_registry_remove(
    UmiAiMcpServerRegistry *registry,
    const char *server_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || server_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                registry->servers[index].server_id,
                server_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find ai mcp server registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_at(
    const UmiAiMcpServerRegistry *registry,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }

    return &registry->servers[index];
}
