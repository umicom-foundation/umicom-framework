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

/*
 * A single registry makes tool discovery deterministic and lets policy inspect the exact tool before execution.
 */

#ifndef INCLUDE_UMICOM_AI_TOOL_REGISTRY_H
#define INCLUDE_UMICOM_AI_TOOL_REGISTRY_H

#include <stddef.h>
#include "umicom/ai/tool.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai tool registry data shared with callers of this public contract.
 */
typedef struct UmiAiToolRegistry {
    UmiAiTool tools[UMI_AI_MAX_TOOLS];
    size_t count;
} UmiAiToolRegistry;

/**
 * Initialise ai tool registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_tool_registry_init(UmiAiToolRegistry *registry);
/**
 * Add ai tool registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_tool_registry_add(UmiAiToolRegistry *registry,
                                   const UmiAiTool *tool);
/**
 * Find ai tool registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiAiTool *umi_ai_tool_registry_find(UmiAiToolRegistry *registry,
                                     const char *tool_id);

/* Validate active tools, the declared count and unique identifiers. NULL is
 * INVALID_ARGUMENT; malformed catalogue storage is INVALID_STATE. Validation
 * does not inspect inactive slots or take ownership of callback user_data. */
UmiStatus UmiAiToolRegistryValidate(const UmiAiToolRegistry *registry);

/* Resolve one borrowed entry only after full catalogue admission. Failure
 * preserves out_tool; missing identifiers return NOT_FOUND. tool_id is a
 * readable terminated string shorter than UMI_AI_ID_CAPACITY. Writable output
 * must not overlap registry storage. Serialize use with every mutation. */
UmiStatus UmiAiToolRegistryResolve(UmiAiToolRegistry *registry,
    const char *tool_id, UmiAiTool **out_tool);

#ifdef __cplusplus
}
#endif

#endif
