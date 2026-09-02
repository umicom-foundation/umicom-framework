/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_activator.c
 *
 * PURPOSE:
 *   Implement Application Shell activation/focus after successful IDE commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_activator.h"

/*
 * Provide the studio surface activate kind operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_surface_activate_kind(
    UmiStudioRuntimeBindings *bindings,
    UmiStudioRuntimeSurfaceKind kind,
    int make_visible,
    int focus)
{
    const UmiStudioRuntimeSurfaceBinding *surface;
    UmiApplicationShellContribution contribution;
    UmiApplicationShellPlacement placement;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    surface = umi_studio_surface_catalogue_for_kind(kind);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_studio_runtime_surface_resolve(
        bindings->shell_registry,
        surface,
        &contribution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this operation only while the related capability or state is available. */
    if (make_visible) {
        status = umi_application_shell_registry_set_state(
            bindings->shell_registry,
            contribution.contribution_id,
            1,
            (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U,
            (contribution.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U,
            contribution.badge_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_application_shell_layout_find(
            bindings->shell_layout,
            contribution.contribution_id,
            &placement);

        /*
         * A contribution can legitimately exist in the shell registry before it
         * has ever been placed in the semantic layout. Activating such a surface
         * must create its placement rather than silently leaving it invisible to
         * the renderer. Existing hidden placements are likewise made visible.
         */
        if (status == UMI_STATUS_NOT_FOUND ||
            (status == UMI_STATUS_OK && !placement.visible)) {
            status = umi_application_shell_layout_place(
                bindings->shell_layout,
                contribution.contribution_id,
                surface->container_id != NULL ? surface->container_id : "",
                surface->region,
                (size_t)(surface->order >= 0 ? surface->order : 0),
                1);
        }

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    status = umi_application_shell_state_activate(
        bindings->shell_state,
        surface->activity_id != NULL ? surface->activity_id : "",
        contribution.contribution_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Apply this branch only when its contract condition is satisfied. */
    if (focus) {
        status = umi_application_shell_state_focus(
            bindings->shell_state,
            contribution.contribution_id);
    }

    return status;
}

/*
 * Provide the studio surface activate for command operation used by this module and its
 * client applications.
 */
UmiStatus umi_studio_surface_activate_for_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id)
{
    UmiStudioRuntimeActivationRule rule;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_activation_resolve(command_id, &rule);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_studio_surface_activate_kind(
        bindings,
        rule.surface,
        rule.make_visible,
        rule.focus);
}
