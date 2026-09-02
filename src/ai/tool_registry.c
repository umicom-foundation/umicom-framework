/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/tool_registry.c
 *
 * PURPOSE:
 *   Register and resolve AI tools by identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The fixed registry keeps tool discovery simple and deterministic for the first stable C ABI.
 */

#include "umicom/ai/tool_registry.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai tool registry from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_tool_registry_init(UmiAiToolRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

/* Add ai tool registry only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_tool_registry_add(UmiAiToolRegistry *registry,
                                   const UmiAiTool *tool)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || tool == NULL || tool->tool_id[0] == '\0' ||
        tool->invoke == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ai_tool_registry_find(registry, tool->tool_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_AI_MAX_TOOLS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->tools[registry->count++] = *tool;
    return UMI_STATUS_OK;
}

/*
 * Find ai tool registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiAiTool *umi_ai_tool_registry_find(UmiAiToolRegistry *registry,
                                     const char *tool_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || tool_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->tools[index].tool_id, tool_id) == 0) {
            return &registry->tools[index];
        }
    }
    return NULL;
}
