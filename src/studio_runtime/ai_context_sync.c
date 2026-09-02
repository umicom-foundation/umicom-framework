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

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the studio ai context sync operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_ai_context_sync(
    const UmiStudioRuntimeSelectionRouter *selection,
    const char *actor_id,
    UmiAiDeveloperCommandContext *out_context)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || actor_id == NULL || out_context == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_ai_developer_command_context_init(out_context);

    status = copy_text(
        out_context->approved_by,
        sizeof(out_context->approved_by),
        actor_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (selection->state.current.kind ==
        UMI_STUDIO_SELECTION_AI_APPROVAL) {
        status = copy_text(
            out_context->approval_id,
            sizeof(out_context->approval_id),
            selection->state.current.subject_id);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (selection->state.current.kind ==
               UMI_STUDIO_SELECTION_AI_PATCH_FILE) {
        status = copy_text(
            out_context->patch_id,
            sizeof(out_context->patch_id),
            selection->state.current.subject_id);
        out_context->file_index = selection->state.current.index;
    }

    return status;
}
