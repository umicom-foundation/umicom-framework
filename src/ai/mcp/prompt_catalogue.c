/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/prompt_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded MCP prompts catalogue registration and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/prompt_catalogue.h"

#include <string.h>

void umi_ai_mcp_prompt_catalogue_init(
    UmiAiMcpPromptCatalogue *catalogue)
{
    if (catalogue == NULL) {
        return;
    }

    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

const UmiAiMcpPromptDescriptor *umi_ai_mcp_prompt_catalogue_find(
    const UmiAiMcpPromptCatalogue *catalogue,
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

UmiStatus umi_ai_mcp_prompt_catalogue_add(
    UmiAiMcpPromptCatalogue *catalogue,
    const UmiAiMcpPromptDescriptor *item)
{
    if (catalogue == NULL ||
        item == NULL ||
        item->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_ai_mcp_prompt_catalogue_find(
            catalogue,
            item->name) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (catalogue->count >= UMI_AI_MCP_MAX_PROMPTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    catalogue->items[catalogue->count++] = *item;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiAiMcpPromptDescriptor *umi_ai_mcp_prompt_catalogue_at(
    const UmiAiMcpPromptCatalogue *catalogue,
    size_t index)
{
    if (catalogue == NULL || index >= catalogue->count) {
        return NULL;
    }

    return &catalogue->items[index];
}
