/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/shell_state.c
 *
 * PURPOSE:
 *   Project current Application Shell active/focus/region state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/shell_state.h"

/*
 * Initialise studio shell state view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_shell_state_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeBindings *bindings;
    UmiApplicationShellState *state;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_studio_runtime_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || bindings->shell_state == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    state = bindings->shell_state;

    status = umi_studio_view_create_base(
        view_id,
        "studio-shell-state",
        "Shell State",
        "Active/focused Application Shell surface, perspective and region visibility.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_string(
        *out_view, "studio.shell.activity", state->active_activity_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.shell.view", state->active_view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.shell.focus", state->focused_view_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.shell.perspective", state->active_perspective_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.shell.primary", state->primary_sidebar_visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.shell.secondary", state->secondary_sidebar_visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.shell.bottom", state->bottom_panel_visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.shell.status", state->status_bar_visible);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.shell.zen", state->zen_mode);

    return status;
}
