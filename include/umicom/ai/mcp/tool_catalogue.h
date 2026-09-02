/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/tool_catalogue.h
 *
 * PURPOSE:
 *   Store bounded MCP tools discovered from one or more servers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_TOOL_CATALOGUE_H
#define UMICOM_AI_MCP_TOOL_CATALOGUE_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai mcp tool catalogue data shared with callers of this public contract.
 */
typedef struct UmiAiMcpToolCatalogue {
    UmiAiMcpToolDescriptor items[UMI_AI_MCP_MAX_TOOLS];
    size_t count;
    uint64_t revision;
} UmiAiMcpToolCatalogue;

/**
 * Initialise ai mcp tool catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_mcp_tool_catalogue_init(
    UmiAiMcpToolCatalogue *catalogue);

/**
 * Add ai mcp tool catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_mcp_tool_catalogue_add(
    UmiAiMcpToolCatalogue *catalogue,
    const UmiAiMcpToolDescriptor *item);

/**
 * Find ai mcp tool catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_find(
    const UmiAiMcpToolCatalogue *catalogue,
    const char *identifier);

/**
 * Find ai mcp tool catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_at(
    const UmiAiMcpToolCatalogue *catalogue,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
