/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/prompt_catalogue.h
 *
 * PURPOSE:
 *   Store bounded MCP prompts discovered from one or more servers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_PROMPT_CATALOGUE_H
#define UMICOM_AI_MCP_PROMPT_CATALOGUE_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai mcp prompt catalogue data shared with callers of this public contract.
 */
typedef struct UmiAiMcpPromptCatalogue {
    UmiAiMcpPromptDescriptor items[UMI_AI_MCP_MAX_PROMPTS];
    size_t count;
    uint64_t revision;
} UmiAiMcpPromptCatalogue;

/**
 * Initialise ai mcp prompt catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_mcp_prompt_catalogue_init(
    UmiAiMcpPromptCatalogue *catalogue);

/**
 * Add ai mcp prompt catalogue only after its inputs and available capacity have been
 * checked.
 */
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_ai_mcp_prompt_catalogue_add(
    UmiAiMcpPromptCatalogue *catalogue,
    const UmiAiMcpPromptDescriptor *item);

/**
 * Find ai mcp prompt catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiMcpPromptDescriptor *umi_ai_mcp_prompt_catalogue_find(
    const UmiAiMcpPromptCatalogue *catalogue,
    const char *identifier);

/**
 * Find ai mcp prompt catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiMcpPromptDescriptor *umi_ai_mcp_prompt_catalogue_at(
    const UmiAiMcpPromptCatalogue *catalogue,
    size_t index);

/* Admit all inline strings and a nonempty identity. Optional text may be empty;
 * URI and schema contents remain metadata, not authorization or validated JSON. */
UmiStatus UmiAiMcpPromptDescriptorValidate(const UmiAiMcpPromptDescriptor *item);
/* Validate count, every active descriptor and unique identities. NULL returns
 * INVALID_ARGUMENT; malformed declared storage returns INVALID_STATE. Unused
 * slots and the revision token do not change this read-only admission result. */
UmiStatus UmiAiMcpPromptCatalogueValidate(const UmiAiMcpPromptCatalogue *catalogue);
/* Resolve a bounded identity after complete catalogue admission. Failure leaves
 * out_item unchanged. NOT_FOUND is distinct from malformed inputs/storage.
 * The returned pointer borrows the catalogue: do not retain it across mutation
 * or destruction, and serialize ownership. Legacy find/at return NULL on error. */
UmiStatus UmiAiMcpPromptCatalogueResolve(const UmiAiMcpPromptCatalogue *catalogue,
    const char *identifier, const UmiAiMcpPromptDescriptor **out_item);

#ifdef __cplusplus
}
#endif
#endif
