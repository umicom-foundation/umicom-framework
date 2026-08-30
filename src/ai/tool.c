/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/tool.c
 *
 * PURPOSE:
 *   Initialise AI tool descriptors with bounded metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The descriptor keeps the callable function, description and permission together so approval UIs can explain what a tool wants to do.
 */

#include "umicom/ai/tool.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

UmiStatus umi_ai_tool_init(UmiAiTool *tool,
                           const char *tool_id,
                           const char *description,
                           const char *permission,
                           UmiAiToolInvoke invoke,
                           void *user_data)
{
    int a;
    int b;
    int c;
    if (tool == NULL || tool_id == NULL || description == NULL ||
        permission == NULL || invoke == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(tool, 0, sizeof(*tool));
    a = snprintf(tool->tool_id, sizeof(tool->tool_id), "%s", tool_id);
    b = snprintf(tool->description, sizeof(tool->description), "%s", description);
    c = snprintf(tool->permission, sizeof(tool->permission), "%s", permission);
    if (a < 0 || b < 0 || c < 0 ||
        (size_t)a >= sizeof(tool->tool_id) ||
        (size_t)b >= sizeof(tool->description) ||
        (size_t)c >= sizeof(tool->permission)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    tool->invoke = invoke;
    tool->user_data = user_data;
    return UMI_STATUS_OK;
}
