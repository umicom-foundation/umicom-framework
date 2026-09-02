/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/presentation_state.c
 *
 * PURPOSE:
 *   Implement bounded AI developer presentation selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/presentation_state.h"

#include <string.h>

/* Provide the set text operation used by this module and its client applications. */
static UmiStatus set_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || capacity == 0U || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ai developer presentation state from caller-provided values so later
 * operations receive a known state.
 */
void umi_ai_developer_presentation_state_init(
    UmiAiDeveloperPresentationState *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;

    (void)memset(state, 0, sizeof(*state));
    state->active_pane = UMI_AI_DEVELOPER_PANE_OVERVIEW;
    state->show_tool_details = 1;
    state->show_validation_output = 1;
    state->revision = 1U;
}

/*
 * Provide the ai developer presentation set pane operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_pane(
    UmiAiDeveloperPresentationState *state,
    UmiAiDeveloperPaneKind pane)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL ||
        pane < UMI_AI_DEVELOPER_PANE_OVERVIEW ||
        pane > UMI_AI_DEVELOPER_PANE_CHECKPOINTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    state->active_pane = pane;
    state->revision += 1U;
    return UMI_STATUS_OK;
}

#define TEXT_SETTER(function_name, field_name) \
UmiStatus function_name( \
    UmiAiDeveloperPresentationState *state, \
    const char *value) \
{ \
    UmiStatus status; \
    if (state == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT; \
    status = set_text(state->field_name, sizeof(state->field_name), value); \
    if (status == UMI_STATUS_OK) state->revision += 1U; \
    return status; \
}

TEXT_SETTER(
    umi_ai_developer_presentation_set_chat,
    active_chat_id)
TEXT_SETTER(
    umi_ai_developer_presentation_set_task,
    active_task_id)
TEXT_SETTER(
    umi_ai_developer_presentation_set_approval,
    active_approval_id)
TEXT_SETTER(
    umi_ai_developer_presentation_set_patch,
    active_patch_id)

#undef TEXT_SETTER

/*
 * Provide the ai developer presentation set diff operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_presentation_set_diff(
    UmiAiDeveloperPresentationState *state,
    const char *path,
    size_t line_index)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = set_text(
        state->active_diff_path,
        sizeof(state->active_diff_path),
        path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    state->active_diff_line = line_index;
    state->revision += 1U;
    return UMI_STATUS_OK;
}
