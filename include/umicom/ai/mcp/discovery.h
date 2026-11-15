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

/* Each discovery operation constructs a fresh catalogue privately. Failure
 * preserves the supplied catalogue, including admission, allocation, transport,
 * decode, duplicate and capacity failures. Fresh destination storage need not
 * be initialized. Success keeps the existing fresh-catalogue revision semantics:
 * its token is not a monotonic identity across repeated discovery operations.
 * A changed session revision or readiness during transport refuses publication.
 * Serialize the complete call; callbacks must keep session/output owners alive
 * and must not mutate the destination catalogue or session configuration. This
 * check is not a lifetime lock or proof of remote-server snapshot consistency. */

/**
 * Provide the ai mcp discover tools operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_discover_tools(
    UmiAiMcpSession *session,
    UmiAiMcpToolCatalogue *catalogue);

/**
 * Provide the ai mcp discover resources operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_discover_resources(
    UmiAiMcpSession *session,
    UmiAiMcpResourceCatalogue *catalogue);

/**
 * Provide the ai mcp discover prompts operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_mcp_discover_prompts(
    UmiAiMcpSession *session,
    UmiAiMcpPromptCatalogue *catalogue);

#ifdef __cplusplus
}
#endif
#endif
