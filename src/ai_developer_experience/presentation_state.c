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

static UmiStatus set_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    if (out == NULL || capacity == 0U || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

void umi_ai_developer_presentation_state_init(
    UmiAiDeveloperPresentationState *state)
{
    if (state == NULL) return;

    (void)memset(state, 0, sizeof(*state));
    state->active_pane = UMI_AI_DEVELOPER_PANE_OVERVIEW;
    state->show_tool_details = 1;
    state->show_validation_output = 1;
    state->revision = 1U;
}

UmiStatus umi_ai_developer_presentation_set_pane(
    UmiAiDeveloperPresentationState *state,
    UmiAiDeveloperPaneKind pane)
{
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

UmiStatus umi_ai_developer_presentation_set_diff(
    UmiAiDeveloperPresentationState *state,
    const char *path,
    size_t line_index)
{
    UmiStatus status;

    if (state == NULL || path == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = set_text(
        state->active_diff_path,
        sizeof(state->active_diff_path),
        path);
    if (status != UMI_STATUS_OK) return status;

    state->active_diff_line = line_index;
    state->revision += 1U;
    return UMI_STATUS_OK;
}
