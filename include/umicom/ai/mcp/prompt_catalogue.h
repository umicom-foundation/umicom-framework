/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/mcp/prompt_catalogue.h
 *
 * PURPOSE:
 *   Store bounded MCP prompts discovered from one or more servers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_MCP_PROMPT_CATALOGUE_H
#define UMICOM_AI_MCP_PROMPT_CATALOGUE_H

#include "umicom/ai/mcp/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiMcpPromptCatalogue {
    UmiAiMcpPromptDescriptor items[UMI_AI_MCP_MAX_PROMPTS];
    size_t count;
    uint64_t revision;
} UmiAiMcpPromptCatalogue;

void umi_ai_mcp_prompt_catalogue_init(
    UmiAiMcpPromptCatalogue *catalogue);

UmiStatus umi_ai_mcp_prompt_catalogue_add(
    UmiAiMcpPromptCatalogue *catalogue,
    const UmiAiMcpPromptDescriptor *item);

const UmiAiMcpPromptDescriptor *umi_ai_mcp_prompt_catalogue_find(
    const UmiAiMcpPromptCatalogue *catalogue,
    const char *identifier);

const UmiAiMcpPromptDescriptor *umi_ai_mcp_prompt_catalogue_at(
    const UmiAiMcpPromptCatalogue *catalogue,
    size_t index);

#ifdef __cplusplus
}
#endif
#endif
