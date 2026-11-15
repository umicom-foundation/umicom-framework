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

#include <stdint.h>
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
/* Validate complete MCP metadata and declared storage before lookup so malformed later entries cannot be hidden. The former implementation is retained for owner review. */
#if 0
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

#endif
/* Schema text is bounded metadata here. JSON and server capability semantics
 * belong to protocol admission; catalogue validation does not execute tools. */
UmiStatus UmiAiMcpToolDescriptorValidate(const UmiAiMcpToolDescriptor *item)
{
    if (item == NULL || item->name[0] == '\0' ||
        memchr(item->name, '\0', sizeof(item->name)) == NULL ||
        memchr(item->description, '\0', sizeof(item->description)) == NULL ||
        memchr(item->input_schema, '\0', sizeof(item->input_schema)) == NULL ||
        memchr(item->permission, '\0', sizeof(item->permission)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiMcpToolCatalogueValidate(const UmiAiMcpToolCatalogue *catalogue)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->count > UMI_AI_MCP_MAX_TOOLS) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < catalogue->count; ++index) {
        if (UmiAiMcpToolDescriptorValidate(&catalogue->items[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t earlier = 0U; earlier < index; ++earlier)
            if (strcmp(catalogue->items[earlier].name, catalogue->items[index].name) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_find(
    const UmiAiMcpToolCatalogue *catalogue, const char *identifier)
{
    if (identifier == NULL || identifier[0] == '\0' ||
        UmiAiMcpToolCatalogueValidate(catalogue) != UMI_STATUS_OK) return NULL;
    size_t length = 0U;
    while (length < UMI_AI_ID_CAPACITY && identifier[length] != '\0') ++length;
    if (length == UMI_AI_ID_CAPACITY) return NULL;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->items[index].name, identifier) == 0)
            return &catalogue->items[index];
    return NULL;
}
/*
 * Add ai mcp tool catalogue only after its inputs and available capacity have been
 * checked.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
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
#endif

/* Shared catalogue admission preserves capacity and revision refusals before publishing a new descriptor. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_tool_catalogue_add(
    UmiAiMcpToolCatalogue *catalogue,
    const UmiAiMcpToolDescriptor *item)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (catalogue != NULL && catalogue->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

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

#endif
UmiStatus umi_ai_mcp_tool_catalogue_add(UmiAiMcpToolCatalogue *catalogue,
    const UmiAiMcpToolDescriptor *item)
{
    /* Preserve the existing exhausted-revision refusal before all other work. */
    if (catalogue != NULL && catalogue->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiMcpToolDescriptorValidate(item);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiMcpToolCatalogueValidate(catalogue);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->items[index].name, item->name) == 0)
            return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count == UMI_AI_MCP_MAX_TOOLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiMcpToolDescriptor candidate = *item;
    catalogue->items[catalogue->count] = candidate;
    ++catalogue->count;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}
/*
 * Find ai mcp tool catalogue while leaving the underlying catalogue or model owned by this
 * module.
 */
/* Indexed access admits the declared catalogue before exposing borrowed storage. The former implementation is retained for owner review. */
#if 0
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

#endif
const UmiAiMcpToolDescriptor *umi_ai_mcp_tool_catalogue_at(
    const UmiAiMcpToolCatalogue *catalogue, size_t index)
{
    if (UmiAiMcpToolCatalogueValidate(catalogue) != UMI_STATUS_OK || index >= catalogue->count)
        return NULL;
    return &catalogue->items[index];
}