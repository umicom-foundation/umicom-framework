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

    if (bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    surface = umi_studio_surface_catalogue_for_kind(kind);
    if (surface == NULL) return UMI_STATUS_NOT_FOUND;

    status = umi_studio_runtime_surface_resolve(
        bindings->shell_registry,
        surface,
        &contribution);
    if (status != UMI_STATUS_OK) return status;

    if (make_visible) {
        status = umi_application_shell_registry_set_state(
            bindings->shell_registry,
            contribution.contribution_id,
            1,
            (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U,
            (contribution.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U,
            contribution.badge_count);
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

        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    status = umi_application_shell_state_activate(
        bindings->shell_state,
        surface->activity_id != NULL ? surface->activity_id : "",
        contribution.contribution_id);
    if (status != UMI_STATUS_OK) return status;

    if (focus) {
        status = umi_application_shell_state_focus(
            bindings->shell_state,
            contribution.contribution_id);
    }

    return status;
}

UmiStatus umi_studio_surface_activate_for_command(
    UmiStudioRuntimeBindings *bindings,
    const char *command_id)
{
    UmiStudioRuntimeActivationRule rule;
    UmiStatus status;

    if (bindings == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_studio_activation_resolve(command_id, &rule);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status != UMI_STATUS_OK) return status;

    return umi_studio_surface_activate_kind(
        bindings,
        rule.surface,
        rule.make_visible,
        rule.focus);
}
