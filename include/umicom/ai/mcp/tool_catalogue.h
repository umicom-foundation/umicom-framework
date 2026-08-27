/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/tool_catalogue.h
 *
 * PURPOSE:
 *   Store bounded MCP tools discovered from one or more servers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_TOOL_CATALOGUE_H
#define UMICOM_AI_MCP_TOOL_CATALOGUE_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiMcpToolCatalogue {
    UmiAiMcpToolDescriptor items[UMI_AI_MCP_MAX_TOOLS];
    size_t count;
    uint64_t revision;
} UmiAiMcpToolCatalogue;

void umi_ai_mcp_tool_catalogue_init(
    UmiAiMcpToolCatalogue *catalogue);

UmiStatus umi_ai_mcp_tool_catalogue_add(
    UmiAiMcpToolCatalogue *catalogue,
    const UmiAiMcpToolDescriptor *item);

const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_find(
    const UmiAiMcpToolCatalogue *catalogue,
    const char *identifier);

const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_at(
    const UmiAiMcpToolCatalogue *catalogue,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
