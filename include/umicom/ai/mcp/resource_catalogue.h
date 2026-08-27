/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/resource_catalogue.h
 *
 * PURPOSE:
 *   Store bounded MCP resources discovered from one or more servers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_RESOURCE_CATALOGUE_H
#define UMICOM_AI_MCP_RESOURCE_CATALOGUE_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiMcpResourceCatalogue {
    UmiAiMcpResourceDescriptor items[UMI_AI_MCP_MAX_RESOURCES];
    size_t count;
    uint64_t revision;
} UmiAiMcpResourceCatalogue;

void umi_ai_mcp_resource_catalogue_init(
    UmiAiMcpResourceCatalogue *catalogue);

UmiStatus umi_ai_mcp_resource_catalogue_add(
    UmiAiMcpResourceCatalogue *catalogue,
    const UmiAiMcpResourceDescriptor *item);

const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_find(
    const UmiAiMcpResourceCatalogue *catalogue,
    const char *identifier);

const UmiAiMcpResourceDescriptor *umi_ai_mcp_resource_catalogue_at(
    const UmiAiMcpResourceCatalogue *catalogue,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
