/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/tool_bridge.h
 *
 * PURPOSE:
 *   Expose discovered MCP tools through the existing UmiAiToolRegistry so current AI policy and approval checks remain authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_TOOL_BRIDGE_H
#define UMICOM_AI_MCP_TOOL_BRIDGE_H

#include "umicom/ai/mcp/invocation.h"
#include "umicom/ai/tool_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai mcp tool binding data shared with callers of this public contract.
 */
typedef struct UmiAiMcpToolBinding {
    UmiAiMcpSession *session;
    char remote_name[UMI_AI_ID_CAPACITY];
} UmiAiMcpToolBinding;

/**
 * Represent the ai mcp tool bridge data shared with callers of this public contract.
 */
typedef struct UmiAiMcpToolBridge {
    UmiAiMcpToolBinding bindings[UMI_AI_MCP_MAX_TOOL_BINDINGS];
    size_t binding_count;
} UmiAiMcpToolBridge;

/*
 * Register discovered MCP tools into the existing AI tool registry. The local
 * id is mcp.<server-id>.<tool-name>. Invocation still passes through
 * umi_ai_runtime_invoke_tool(), so UmiAiPolicy approval remains in force.
 */
UmiStatus umi_ai_mcp_tool_bridge_register(
    UmiAiMcpToolBridge *bridge,
    UmiAiMcpSession *session,
    const UmiAiMcpToolCatalogue *catalogue,
    UmiAiToolRegistry *tool_registry);

#ifdef __cplusplus
}
#endif
#endif
