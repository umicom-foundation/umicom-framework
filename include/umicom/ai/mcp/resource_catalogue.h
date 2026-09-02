/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/resource_catalogue.h
 *
 * PURPOSE:
 *   Store bounded MCP resources discovered from one or more servers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_RESOURCE_CATALOGUE_H
#define UMICOM_AI_MCP_RESOURCE_CATALOGUE_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai mcp resource catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiAiMcpResourceCatalogue {
    UmiAiMcpResourceDescriptor items[UMI_AI_MCP_MAX_RESOURCES];
    size_t count;
    uint64_t revision;
} UmiAiMcpResourceCatalogue;

/**
 * Initialise ai mcp resource catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_mcp_resource_catalogue_init(
    UmiAiMcpResourceCatalogue *catalogue);

/**
 * Add ai mcp resource catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_mcp_resource_catalogue_add(
    UmiAiMcpResourceCatalogue *catalogue,
    const UmiAiMcpResourceDescriptor *item);

/**
 * Find ai mcp resource catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_find(
    const UmiAiMcpResourceCatalogue *catalogue,
    const char *identifier);

/**
 * Find ai mcp resource catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_at(
    const UmiAiMcpResourceCatalogue *catalogue,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
