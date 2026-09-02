/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/tool_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded MCP tools catalogue registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/mcp/tool_catalogue.h"

#include <string.h>

/*
 * Initialise ai mcp tool catalogue from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_mcp_tool_catalogue_init(
    UmiAiMcpToolCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) {
        return;
    }

    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

/*
 * Find ai mcp tool catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_find(
    const UmiAiMcpToolCatalogue *catalogue,
    const char *identifier)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || identifier == NULL) {
        return NULL;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                catalogue->items[index].name,
                identifier) == 0) {
            return &catalogue->items[index];
        }
    }

    return NULL;
}

/*
 * Add ai mcp tool catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_mcp_tool_catalogue_add(
    UmiAiMcpToolCatalogue *catalogue,
    const UmiAiMcpToolDescriptor *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL ||
        item == NULL ||
        item->name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_mcp_tool_catalogue_find(
            catalogue,
            item->name) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_AI_MCP_MAX_TOOLS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    catalogue->items[catalogue->count++] = *item;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ai mcp tool catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_at(
    const UmiAiMcpToolCatalogue *catalogue,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || index >= catalogue->count) {
        return NULL;
    }

    return &catalogue->items[index];
}
