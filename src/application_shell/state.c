/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/state.c
 *
 * PURPOSE:
 *   Implement toolkit-neutral application shell focus and visibility state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/state.h"

#include <string.h>

/* Provide the copy id operation used by this module and its client applications. */
static UmiStatus copy_id(char *destination,
                         size_t capacity,
                         const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U ||
        source == NULL || source[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise application shell state from caller-provided values so later operations
 * receive a known state.
 */
void umi_application_shell_state_init(UmiApplicationShellState *state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;

    (void)memset(state, 0, sizeof(*state));
    state->primary_sidebar_visible = 1;
    state->bottom_panel_visible = 1;
    state->status_bar_visible = 1;
    state->revision = 1U;
}

/*
 * Provide the application shell state activate operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_state_activate(
    UmiApplicationShellState *state,
    const char *activity_id,
    const char *view_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (activity_id != NULL && activity_id[0] != '\0') {
        status = copy_id(state->active_activity_id,
                         sizeof(state->active_activity_id),
                         activity_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id != NULL && view_id[0] != '\0') {
        status = copy_id(state->active_view_id,
                         sizeof(state->active_view_id),
                         view_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    state->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application shell state focus operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_shell_state_focus(
    UmiApplicationShellState *state,
    const char *view_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = copy_id(state->focused_view_id,
                     sizeof(state->focused_view_id),
                     view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) state->revision += 1U;
    return status;
}

/*
 * Provide the application shell state set perspective operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_shell_state_set_perspective(
    UmiApplicationShellState *state,
    const char *perspective_id)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = copy_id(state->active_perspective_id,
                     sizeof(state->active_perspective_id),
                     perspective_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) state->revision += 1U;
    return status;
}

/*
 * Provide the application shell state set regions operation used by this module and its
 * client applications.
 */
void umi_application_shell_state_set_regions(
    UmiApplicationShellState *state,
    int primary_sidebar_visible,
    int secondary_sidebar_visible,
    int bottom_panel_visible,
    int status_bar_visible)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;

    state->primary_sidebar_visible = primary_sidebar_visible != 0;
    state->secondary_sidebar_visible = secondary_sidebar_visible != 0;
    state->bottom_panel_visible = bottom_panel_visible != 0;
    state->status_bar_visible = status_bar_visible != 0;
    state->revision += 1U;
}

/*
 * Provide the application shell state set zen operation used by this module and its client
 * applications.
 */
void umi_application_shell_state_set_zen(
    UmiApplicationShellState *state,
    int zen_mode)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (state == NULL) return;

    state->zen_mode = zen_mode != 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (state->zen_mode) {
        state->primary_sidebar_visible = 0;
        state->secondary_sidebar_visible = 0;
        state->bottom_panel_visible = 0;
        state->status_bar_visible = 0;
    }
    state->revision += 1U;
}
