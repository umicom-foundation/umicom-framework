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
#include <stdint.h>

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
/* Validate complete bounded registry storage before returning a borrowed match, while retaining the mutable compatibility interface. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Admit all active entries before returning any borrowed metadata. A matching
 * early identity must not hide malformed storage later in the registry. */
UmiStatus UmiAiMcpServerRegistryValidate(const UmiAiMcpServerRegistry *registry)
{
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count > UMI_AI_MCP_MAX_SERVERS) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (umi_ai_mcp_server_validate(&registry->servers[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t earlier = 0U; earlier < index; ++earlier)
            if (strcmp(registry->servers[earlier].server_id, registry->servers[index].server_id) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiAiMcpServerRegistryResolve(const UmiAiMcpServerRegistry *registry,
    const char *server_id, const UmiAiMcpServerDescriptor **out_server)
{
    if (registry == NULL || server_id == NULL || out_server == NULL || server_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_ID_CAPACITY && server_id[length] != '\0') ++length;
    if (length == UMI_AI_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiMcpServerRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->servers[index].server_id, server_id) == 0) {
            *out_server = &registry->servers[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_find(UmiAiMcpServerRegistry *registry, const char *server_id)
{
    const UmiAiMcpServerDescriptor *entry = NULL;
    if (UmiAiMcpServerRegistryResolve(registry, server_id, &entry) != UMI_STATUS_OK) return NULL;
    /* The compatibility caller supplied a mutable registry; recover its matching
     * element without casting away const from an independently owned object. */
    return &registry->servers[entry - registry->servers];
}

/*
 * Add ai mcp server registry only after its inputs and available capacity have been
 * checked.
 */
/* Admit the candidate and complete registry, refuse revision exhaustion and stage insertion before any write. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus umi_ai_mcp_server_registry_add(UmiAiMcpServerRegistry *registry,
    const UmiAiMcpServerDescriptor *server)
{
    if (registry != NULL && registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_ai_mcp_server_validate(server);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiMcpServerRegistryValidate(registry);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < registry->count; ++index)
        if (strcmp(registry->servers[index].server_id, server->server_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count == UMI_AI_MCP_MAX_SERVERS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiMcpServerDescriptor candidate = *server;
    registry->servers[registry->count] = candidate;
    ++registry->count; ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove ai mcp server registry while keeping the remaining records in a valid and
 * discoverable state.
 */
/* Resolve an admitted entry and refuse revision exhaustion before compacting active storage; retain the existing unused-tail semantics. The former implementation is retained for owner review. */
#if 0
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

#endif
UmiStatus umi_ai_mcp_server_registry_remove(UmiAiMcpServerRegistry *registry, const char *server_id)
{
    if (registry != NULL && registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiMcpServerDescriptor *entry = NULL;
    UmiStatus status = UmiAiMcpServerRegistryResolve(registry, server_id, &entry);
    if (status != UMI_STATUS_OK) return status;
    size_t index = (size_t)(entry - registry->servers);
    if (index + 1U < registry->count)
        memmove(&registry->servers[index], &registry->servers[index + 1U],
            (registry->count - index - 1U) * sizeof(registry->servers[0]));
    --registry->count; ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Find ai mcp server registry while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Validate every active record and the declared count before exposing indexed borrowed storage. The former implementation is retained for owner review. */
#if 0
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

#endif
const UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_at(const UmiAiMcpServerRegistry *registry, size_t index)
{
    if (UmiAiMcpServerRegistryValidate(registry) != UMI_STATUS_OK || index >= registry->count) return NULL;
    return &registry->servers[index];
}
