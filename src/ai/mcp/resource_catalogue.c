/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/mcp/resource_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded MCP resources catalogue registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdint.h>
#include "umicom/ai/mcp/resource_catalogue.h"

#include <string.h>

/*
 * Initialise ai mcp resource catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_mcp_resource_catalogue_init(
    UmiAiMcpResourceCatalogue *catalogue)
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
 * Find ai mcp resource catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Validate declared catalogue storage and resolve only complete active metadata; preserve the pointer-returning API. The former implementation is retained for owner review. */
#if 0
const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_find(
    const UmiAiMcpResourceCatalogue *catalogue,
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
                catalogue->items[index].uri,
                identifier) == 0) {
            return &catalogue->items[index];
        }
    }

    return NULL;
}

#endif
/* Inline strings are admitted before comparison or presentation. Optional text
 * may be empty; this storage check does not validate URI or schema semantics. */
UmiStatus UmiAiMcpResourceDescriptorValidate(const UmiAiMcpResourceDescriptor *item)
{
    if (item == NULL || item->uri[0] == '\0' ||
        memchr(item->uri, '\0', sizeof(item->uri)) == NULL ||
        memchr(item->name, '\0', sizeof(item->name)) == NULL ||
        memchr(item->description, '\0', sizeof(item->description)) == NULL ||
        memchr(item->mime_type, '\0', sizeof(item->mime_type)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiMcpResourceCatalogueValidate(const UmiAiMcpResourceCatalogue *catalogue)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->count > UMI_AI_MCP_MAX_RESOURCES) return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < catalogue->count; ++index) {
        if (UmiAiMcpResourceDescriptorValidate(&catalogue->items[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_STATE;
        for (size_t earlier = 0U; earlier < index; ++earlier)
            if (strcmp(catalogue->items[earlier].uri, catalogue->items[index].uri) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiMcpResourceCatalogueResolve(const UmiAiMcpResourceCatalogue *catalogue,
    const char *identifier, const UmiAiMcpResourceDescriptor **out_item)
{
    if (catalogue == NULL || identifier == NULL || out_item == NULL || identifier[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_TEXT_CAPACITY && identifier[length] != '\0') ++length;
    if (length == UMI_AI_TEXT_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiMcpResourceCatalogueValidate(catalogue);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].uri, identifier) == 0) {
            *out_item = &catalogue->items[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_find(const UmiAiMcpResourceCatalogue *catalogue, const char *identifier)
{
    const UmiAiMcpResourceDescriptor *item = NULL;
    return UmiAiMcpResourceCatalogueResolve(catalogue, identifier, &item) == UMI_STATUS_OK ? item : NULL;
}

/*
 * Add ai mcp resource catalogue only after its inputs and available capacity have been
 * checked.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_ai_mcp_resource_catalogue_add(
    UmiAiMcpResourceCatalogue *catalogue,
    const UmiAiMcpResourceDescriptor *item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL ||
        item == NULL ||
        item->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_mcp_resource_catalogue_find(
            catalogue,
            item->uri) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_AI_MCP_MAX_RESOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    catalogue->items[catalogue->count++] = *item;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

/* Admit the candidate and existing catalogue before staging a new descriptor without changing revision refusal precedence. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_mcp_resource_catalogue_add(
    UmiAiMcpResourceCatalogue *catalogue,
    const UmiAiMcpResourceDescriptor *item)
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
        item->uri[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ai_mcp_resource_catalogue_find(
            catalogue,
            item->uri) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_AI_MCP_MAX_RESOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    catalogue->items[catalogue->count++] = *item;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_mcp_resource_catalogue_add(UmiAiMcpResourceCatalogue *catalogue, const UmiAiMcpResourceDescriptor *item)
{
    /* Keep the established exhausted-revision refusal before any nested work. */
    if (catalogue != NULL && catalogue->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiMcpResourceDescriptorValidate(item);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiMcpResourceCatalogueValidate(catalogue);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < catalogue->count; ++index)
        if (strcmp(catalogue->items[index].uri, item->uri) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count == UMI_AI_MCP_MAX_RESOURCES) return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiMcpResourceDescriptor candidate = *item;
    catalogue->items[catalogue->count] = candidate;
    ++catalogue->count;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Find ai mcp resource catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
/* Validate the complete catalogue before returning borrowed indexed storage. The former implementation is retained for owner review. */
#if 0
const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_at(
    const UmiAiMcpResourceCatalogue *catalogue,
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
const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_at(const UmiAiMcpResourceCatalogue *catalogue, size_t index)
{
    if (UmiAiMcpResourceCatalogueValidate(catalogue) != UMI_STATUS_OK || index >= catalogue->count) return NULL;
    return &catalogue->items[index];
}
