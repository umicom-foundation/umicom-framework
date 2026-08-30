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

typedef struct UmiAiMcpServerRegistry {
    UmiAiMcpServerDescriptor servers[UMI_AI_MCP_MAX_SERVERS];
    size_t count;
    uint64_t revision;
} UmiAiMcpServerRegistry;

void umi_ai_mcp_server_registry_init(
    UmiAiMcpServerRegistry *registry);

UmiStatus umi_ai_mcp_server_registry_add(
    UmiAiMcpServerRegistry *registry,
    const UmiAiMcpServerDescriptor *server);

UmiStatus umi_ai_mcp_server_registry_remove(
    UmiAiMcpServerRegistry *registry,
    const char *server_id);

UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_find(
    UmiAiMcpServerRegistry *registry,
    const char *server_id);

const UmiAiMcpServerDescriptor *umi_ai_mcp_server_registry_at(
    const UmiAiMcpServerRegistry *registry,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
