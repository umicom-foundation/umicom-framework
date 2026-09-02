/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_preset.c
 *
 * PURPOSE:
 *   Implement semantic Studio preset application over Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_preset.h"

/* Search one semantic preset without exposing its array representation to
 * menus, previews, tests or future presentation adapters. */
int umi_studio_layout_preset_contains_surface(
    const UmiStudioRuntimeLayoutPresetDefinition *preset,
    UmiStudioRuntimeSurfaceKind kind)
{
    size_t index;

    /* A missing preset cannot contain a surface; checking here also makes the
     * public query safe for menu previews and validation tools. */
    if (preset == NULL || preset->visible_surfaces == NULL) return 0;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preset->visible_surface_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preset->visible_surfaces[index] == kind) return 1;
    }
    return 0;
}

/*
 * Check that studio layout preset satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_studio_layout_preset_validate(
    const UmiStudioRuntimeLayoutPresetDefinition *preset)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preset == NULL ||
        preset->preset_id == NULL || preset->preset_id[0] == '\0' ||
        preset->title == NULL || preset->title[0] == '\0' ||
        preset->description == NULL ||
        preset->perspective_id == NULL || preset->perspective_id[0] == '\0' ||
        preset->visible_surfaces == NULL ||
        preset->visible_surface_count == 0U ||
        preset->visible_surface_count > UMI_STUDIO_LAYOUT_PRESET_SURFACE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preset->visible_surface_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preset->visible_surfaces[index] < UMI_STUDIO_SURFACE_EXPLORER ||
            preset->visible_surfaces[index] > UMI_STUDIO_SURFACE_LAST) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }

    return UMI_STATUS_OK;
}

/*
 * Perform studio layout preset through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_studio_layout_preset_apply(
    UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeLayoutPresetDefinition *preset)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_bindings_validate(bindings);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_layout_preset_validate(preset);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *surface =
            umi_studio_surface_catalogue_at(index);
        UmiApplicationShellContribution contribution;
        const int visible =
            surface != NULL && umi_studio_layout_preset_contains_surface(
                                   preset, surface->kind);

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (surface == NULL) continue;

        status = umi_studio_runtime_surface_resolve(
            bindings->shell_registry,
            surface,
            &contribution);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_application_shell_registry_set_state(
            bindings->shell_registry,
            contribution.contribution_id,
            visible,
            (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U,
            (contribution.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U,
            contribution.badge_count);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        status = umi_application_shell_layout_place(
            bindings->shell_layout,
            contribution.contribution_id,
            surface->container_id != NULL ? surface->container_id : "",
            surface->region,
            (size_t)(surface->order >= 0 ? surface->order : 0),
            visible);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_application_shell_state_set_perspective(
        bindings->shell_state,
        preset->perspective_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_application_shell_state_set_regions(
        bindings->shell_state,
        preset->primary_sidebar_visible,
        preset->secondary_sidebar_visible,
        preset->bottom_panel_visible,
        preset->status_bar_visible);
    umi_application_shell_state_set_zen(
        bindings->shell_state,
        preset->zen_mode);

    return UMI_STATUS_OK;
}
