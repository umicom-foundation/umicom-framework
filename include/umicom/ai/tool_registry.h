/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/tool_registry.h
 *
 * PURPOSE:
 *   Register and resolve bounded AI tools by stable identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A single registry makes tool discovery deterministic and lets policy inspect the exact tool before execution.
 */

#ifndef INCLUDE_UMICOM_AI_TOOL_REGISTRY_H
#define INCLUDE_UMICOM_AI_TOOL_REGISTRY_H

#include <stddef.h>
#include "umicom/ai/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiToolRegistry {
    UmiAiTool tools[UMI_AI_MAX_TOOLS];
    size_t count;
} UmiAiToolRegistry;

void umi_ai_tool_registry_init(UmiAiToolRegistry *registry);
UmiStatus umi_ai_tool_registry_add(UmiAiToolRegistry *registry,
                                   const UmiAiTool *tool);
UmiAiTool *umi_ai_tool_registry_find(UmiAiToolRegistry *registry,
                                     const char *tool_id);

#ifdef __cplusplus
}
#endif

#endif
