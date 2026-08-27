/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/resource_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded MCP resources catalogue registration and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/resource_catalogue.h"

#include <string.h>

void umi_ai_mcp_resource_catalogue_init(
    UmiAiMcpResourceCatalogue *catalogue)
{
    if (catalogue == NULL) {
        return;
    }

    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_find(
    const UmiAiMcpResourceCatalogue *catalogue,
    const char *identifier)
{
    size_t index;

    if (catalogue == NULL || identifier == NULL) {
        return NULL;
    }

    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(
                catalogue->items[index].uri,
                identifier) == 0) {
            return &catalogue->items[index];
        }
    }

    return NULL;
}

UmiStatus umi_ai_mcp_resource_catalogue_add(
    UmiAiMcpResourceCatalogue *catalogue,
    const UmiAiMcpResourceDescriptor *item)
{
    if (catalogue == NULL ||
        item == NULL ||
        item->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_ai_mcp_resource_catalogue_find(
            catalogue,
            item->uri) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (catalogue->count >= UMI_AI_MCP_MAX_RESOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    catalogue->items[catalogue->count++] = *item;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_at(
    const UmiAiMcpResourceCatalogue *catalogue,
    size_t index)
{
    if (catalogue == NULL || index >= catalogue->count) {
        return NULL;
    }

    return &catalogue->items[index];
}
