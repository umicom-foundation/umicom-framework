/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/discovery.h
 *
 * PURPOSE:
 *   Discover MCP tools, resources and prompts into bounded Framework catalogues using standard list methods.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_DISCOVERY_H
#define UMICOM_AI_MCP_DISCOVERY_H

#include "umicom/ai/mcp/session.h"
#include "umicom/ai/mcp/tool_catalogue.h"
#include "umicom/ai/mcp/resource_catalogue.h"
#include "umicom/ai/mcp/prompt_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_mcp_discover_tools(
    UmiAiMcpSession *session,
    UmiAiMcpToolCatalogue *catalogue);

UmiStatus umi_ai_mcp_discover_resources(
    UmiAiMcpSession *session,
    UmiAiMcpResourceCatalogue *catalogue);

UmiStatus umi_ai_mcp_discover_prompts(
    UmiAiMcpSession *session,
    UmiAiMcpPromptCatalogue *catalogue);

#ifdef __cplusplus
}
#endif
#endif
