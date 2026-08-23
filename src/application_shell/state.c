/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/state.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral application shell focus and visibility state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/state.h"

#include <string.h>

static UmiStatus copy_id(char *destination,
                         size_t capacity,
                         const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U ||
        source == NULL || source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

void umi_application_shell_state_init(UmiApplicationShellState *state)
{
    if (state == NULL) return;

    (void)memset(state, 0, sizeof(*state));
    state->primary_sidebar_visible = 1;
    state->bottom_panel_visible = 1;
    state->status_bar_visible = 1;
    state->revision = 1U;
}

UmiStatus umi_application_shell_state_activate(
    UmiApplicationShellState *state,
    const char *activity_id,
    const char *view_id)
{
    UmiStatus status;

    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (activity_id != NULL && activity_id[0] != '\0') {
        status = copy_id(state->active_activity_id,
                         sizeof(state->active_activity_id),
                         activity_id);
        if (status != UMI_STATUS_OK) return status;
    }

    if (view_id != NULL && view_id[0] != '\0') {
        status = copy_id(state->active_view_id,
                         sizeof(state->active_view_id),
                         view_id);
        if (status != UMI_STATUS_OK) return status;
    }

    state->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_shell_state_focus(
    UmiApplicationShellState *state,
    const char *view_id)
{
    UmiStatus status;

    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = copy_id(state->focused_view_id,
                     sizeof(state->focused_view_id),
                     view_id);
    if (status == UMI_STATUS_OK) state->revision += 1U;
    return status;
}

UmiStatus umi_application_shell_state_set_perspective(
    UmiApplicationShellState *state,
    const char *perspective_id)
{
    UmiStatus status;

    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = copy_id(state->active_perspective_id,
                     sizeof(state->active_perspective_id),
                     perspective_id);
    if (status == UMI_STATUS_OK) state->revision += 1U;
    return status;
}

void umi_application_shell_state_set_regions(
    UmiApplicationShellState *state,
    int primary_sidebar_visible,
    int secondary_sidebar_visible,
    int bottom_panel_visible,
    int status_bar_visible)
{
    if (state == NULL) return;

    state->primary_sidebar_visible = primary_sidebar_visible != 0;
    state->secondary_sidebar_visible = secondary_sidebar_visible != 0;
    state->bottom_panel_visible = bottom_panel_visible != 0;
    state->status_bar_visible = status_bar_visible != 0;
    state->revision += 1U;
}

void umi_application_shell_state_set_zen(
    UmiApplicationShellState *state,
    int zen_mode)
{
    if (state == NULL) return;

    state->zen_mode = zen_mode != 0;
    if (state->zen_mode) {
        state->primary_sidebar_visible = 0;
        state->secondary_sidebar_visible = 0;
        state->bottom_panel_visible = 0;
        state->status_bar_visible = 0;
    }
    state->revision += 1U;
}
