/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/server_registry.h
 *
 * PURPOSE:
 *   Register bounded MCP server descriptors by stable id without changing the existing AI provider registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_SERVER_REGISTRY_H
#define UMICOM_AI_MCP_SERVER_REGISTRY_H

#include "umicom/ai/mcp/server.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai mcp server registry data shared with callers of this public contract.
 */
typedef struct UmiAiMcpServerRegistry {
    UmiAiMcpServerDescriptor servers[UMI_AI_MCP_MAX_SERVERS];
    size_t count;
    uint64_t revision;
} UmiAiMcpServerRegistry;

/**
 * Initialise ai mcp server registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_mcp_server_registry_init(
    UmiAiMcpServerRegistry *registry);

/**
 * Add ai mcp server registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_mcp_server_registry_add(
    UmiAiMcpServerRegistry *registry,
    const UmiAiMcpServerDescriptor *server);

/**
 * Remove ai mcp server registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ai_mcp_server_registry_remove(
    UmiAiMcpServerRegistry *registry,
    const char *server_id);

/**
 * Find ai mcp server registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_find(
    UmiAiMcpServerRegistry *registry,
    const char *server_id);

/**
 * Find ai mcp server registry while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_at(
    const UmiAiMcpServerRegistry *registry,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
