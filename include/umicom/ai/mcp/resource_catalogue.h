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
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
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

/* Admit all inline strings and a nonempty identity. Optional text may be empty;
 * URI and schema contents remain metadata, not authorization or validated JSON. */
UmiStatus UmiAiMcpResourceDescriptorValidate(const UmiAiMcpResourceDescriptor *item);
/* Validate count, every active descriptor and unique identities. NULL returns
 * INVALID_ARGUMENT; malformed declared storage returns INVALID_STATE. Unused
 * slots and the revision token do not change this read-only admission result. */
UmiStatus UmiAiMcpResourceCatalogueValidate(const UmiAiMcpResourceCatalogue *catalogue);
/* Resolve a bounded identity after complete catalogue admission. Failure leaves
 * out_item unchanged. NOT_FOUND is distinct from malformed inputs/storage.
 * The returned pointer borrows the catalogue: do not retain it across mutation
 * or destruction, and serialize ownership. Legacy find/at return NULL on error. */
UmiStatus UmiAiMcpResourceCatalogueResolve(const UmiAiMcpResourceCatalogue *catalogue,
    const char *identifier, const UmiAiMcpResourceDescriptor **out_item);

#ifdef __cplusplus
}
#endif
#endif
