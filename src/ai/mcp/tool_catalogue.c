/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/tool_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded MCP tools catalogue registration and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/tool_catalogue.h"

#include <string.h>

void umi_ai_mcp_tool_catalogue_init(
    UmiAiMcpToolCatalogue *catalogue)
{
    if (catalogue == NULL) {
        return;
    }

    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_find(
    const UmiAiMcpToolCatalogue *catalogue,
    const char *identifier)
{
    size_t index;

    if (catalogue == NULL || identifier == NULL) {
        return NULL;
    }

    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(
                catalogue->items[index].name,
                identifier) == 0) {
            return &catalogue->items[index];
        }
    }

    return NULL;
}

UmiStatus umi_ai_mcp_tool_catalogue_add(
    UmiAiMcpToolCatalogue *catalogue,
    const UmiAiMcpToolDescriptor *item)
{
    if (catalogue == NULL ||
        item == NULL ||
        item->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_ai_mcp_tool_catalogue_find(
            catalogue,
            item->name) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (catalogue->count >= UMI_AI_MCP_MAX_TOOLS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    catalogue->items[catalogue->count++] = *item;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_at(
    const UmiAiMcpToolCatalogue *catalogue,
    size_t index)
{
    if (catalogue == NULL || index >= catalogue->count) {
        return NULL;
    }

    return &catalogue->items[index];
}
