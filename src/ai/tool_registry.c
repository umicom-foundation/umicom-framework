/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/tool_registry.c
 *
 * PURPOSE:
 *   Register and resolve AI tools by identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The fixed registry keeps tool discovery simple and deterministic for the first stable C ABI.
 */

#include "umicom/ai/tool_registry.h"
#include <stddef.h>

#include <string.h>

void umi_ai_tool_registry_init(UmiAiToolRegistry *registry)
{
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

UmiStatus umi_ai_tool_registry_add(UmiAiToolRegistry *registry,
                                   const UmiAiTool *tool)
{
    if (registry == NULL || tool == NULL || tool->tool_id[0] == '\0' ||
        tool->invoke == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_ai_tool_registry_find(registry, tool->tool_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_AI_MAX_TOOLS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->tools[registry->count++] = *tool;
    return UMI_STATUS_OK;
}

UmiAiTool *umi_ai_tool_registry_find(UmiAiToolRegistry *registry,
                                     const char *tool_id)
{
    size_t index;
    if (registry == NULL || tool_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->tools[index].tool_id, tool_id) == 0) {
            return &registry->tools[index];
        }
    }
    return NULL;
}
