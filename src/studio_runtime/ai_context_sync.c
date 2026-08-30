/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/ai_context_sync.c
 *
 * PURPOSE:
 *   Implement Studio selection -> AI Developer command context projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/ai_context_sync.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    if (out == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_ai_context_sync(
    const UmiStudioRuntimeSelectionRouter *selection,
    const char *actor_id,
    UmiAiDeveloperCommandContext *out_context)
{
    UmiStatus status;

    if (selection == NULL || actor_id == NULL || out_context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_developer_command_context_init(out_context);

    status = copy_text(
        out_context->approved_by,
        sizeof(out_context->approved_by),
        actor_id);
    if (status != UMI_STATUS_OK) return status;

    if (selection->state.current.kind ==
        UMI_STUDIO_SELECTION_AI_APPROVAL) {
        status = copy_text(
            out_context->approval_id,
            sizeof(out_context->approval_id),
            selection->state.current.subject_id);
    } else if (selection->state.current.kind ==
               UMI_STUDIO_SELECTION_AI_PATCH_FILE) {
        status = copy_text(
            out_context->patch_id,
            sizeof(out_context->patch_id),
            selection->state.current.subject_id);
        out_context->file_index = selection->state.current.index;
    }

    return status;
}
