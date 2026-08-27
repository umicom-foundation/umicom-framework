/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/client.h
 *
 * PURPOSE:
 *   Compose one MCP session, discovered catalogues and existing AI tool registry bridge into a reusable Framework client.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_CLIENT_H
#define UMICOM_AI_MCP_CLIENT_H

#include "umicom/ai/mcp/tool_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiMcpClient {
    UmiAiMcpSession session;
    UmiAiMcpToolCatalogue tools;
    UmiAiMcpResourceCatalogue resources;
    UmiAiMcpPromptCatalogue prompts;
    UmiAiMcpToolBridge tool_bridge;
} UmiAiMcpClient;

UmiStatus umi_ai_mcp_client_init(
    UmiAiMcpClient *client,
    const UmiAiMcpServerDescriptor *server,
    const UmiAiMcpTransport *transport);

UmiStatus umi_ai_mcp_client_start(
    UmiAiMcpClient *client,
    const char *client_name,
    const char *client_version);

/*
 * Discover only capabilities actually negotiated by the server. Unsupported
 * catalogues remain empty and are not treated as errors.
 */
UmiStatus umi_ai_mcp_client_discover(
    UmiAiMcpClient *client);

/* Register discovered MCP tools into the existing AI runtime tool registry. */
UmiStatus umi_ai_mcp_client_register_tools(
    UmiAiMcpClient *client,
    UmiAiToolRegistry *tool_registry);

#ifdef __cplusplus
}
#endif
#endif
