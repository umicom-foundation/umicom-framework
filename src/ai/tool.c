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

/*
 * The descriptor keeps the callable function, description and permission together so approval UIs can explain what a tool wants to do.
 */

#include "umicom/ai/tool.h"
#include <stddef.h>

#include <stdio.h>
#include <string.h>

/*
 * Initialise ai tool from caller-provided values so later operations receive a known
 * state.
 */
/* Stage and validate bounded tool metadata before publication so failed initialization and self-aliased text retain the previous value. The former implementation is retained for owner review. */
#if 0
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tool == NULL || tool_id == NULL || description == NULL ||
        permission == NULL || invoke == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(tool, 0, sizeof(*tool));
    a = snprintf(tool->tool_id, sizeof(tool->tool_id), "%s", tool_id);
    b = snprintf(tool->description, sizeof(tool->description), "%s", description);
    c = snprintf(tool->permission, sizeof(tool->permission), "%s", permission);
    /* Apply this branch only when its contract condition is satisfied. */
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

#endif
/* Tool descriptors are copied values; user_data remains borrowed. Validate
 * every inline field before any consumer can compare or display its text.
 * Flags are opaque metadata, not a grant of execution permission. */
UmiStatus UmiAiToolValidate(const UmiAiTool *tool)
{
    if (tool == NULL || tool->invoke == NULL || tool->tool_id[0] == '\0' ||
        memchr(tool->tool_id, '\0', sizeof(tool->tool_id)) == NULL ||
        memchr(tool->description, '\0', sizeof(tool->description)) == NULL ||
        memchr(tool->permission, '\0', sizeof(tool->permission)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Stop at the destination capacity. Staging also permits strings borrowed
 * from the previous destination because publication happens after all reads. */
static UmiStatus CopyToolText(char *destination, size_t capacity, const char *source)
{
    size_t length = 0U;
    while (length < capacity && source[length] != '\0') ++length;
    if (length == capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_tool_init(UmiAiTool *tool, const char *tool_id,
    const char *description, const char *permission, UmiAiToolInvoke invoke,
    void *user_data)
{
    if (tool == NULL || tool_id == NULL || description == NULL ||
        permission == NULL || invoke == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTool candidate = {0};
    UmiStatus status = CopyToolText(candidate.tool_id, sizeof(candidate.tool_id), tool_id);
    if (status == UMI_STATUS_OK)
        status = CopyToolText(candidate.description, sizeof(candidate.description), description);
    if (status == UMI_STATUS_OK)
        status = CopyToolText(candidate.permission, sizeof(candidate.permission), permission);
    if (status != UMI_STATUS_OK) return status;
    candidate.invoke = invoke;
    candidate.user_data = user_data;
    status = UmiAiToolValidate(&candidate);
    if (status != UMI_STATUS_OK) return status;
    *tool = candidate;
    return UMI_STATUS_OK;
}