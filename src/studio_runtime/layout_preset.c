/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_preset.c
 *
 * PURPOSE:
 *   Implement semantic Studio preset application over Application Shell.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_preset.h"

static int contains_surface(
    const UmiStudioRuntimeLayoutPresetDefinition *preset,
    UmiStudioRuntimeSurfaceKind kind)
{
    size_t index;

    for (index = 0U; index < preset->visible_surface_count; ++index) {
        if (preset->visible_surfaces[index] == kind) return 1;
    }
    return 0;
}

UmiStatus umi_studio_layout_preset_validate(
    const UmiStudioRuntimeLayoutPresetDefinition *preset)
{
    size_t index;

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

    for (index = 0U; index < preset->visible_surface_count; ++index) {
        if (preset->visible_surfaces[index] < UMI_STUDIO_SURFACE_EXPLORER ||
            preset->visible_surfaces[index] > UMI_STUDIO_SURFACE_AI_VALIDATION) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_studio_layout_preset_apply(
    UmiStudioRuntimeBindings *bindings,
    const UmiStudioRuntimeLayoutPresetDefinition *preset)
{
    size_t index;
    UmiStatus status;

    if (bindings == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_bindings_validate(bindings);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_layout_preset_validate(preset);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < umi_studio_surface_catalogue_count(); ++index) {
        const UmiStudioRuntimeSurfaceBinding *surface =
            umi_studio_surface_catalogue_at(index);
        UmiApplicationShellContribution contribution;
        const int visible =
            surface != NULL && contains_surface(preset, surface->kind);

        if (surface == NULL) continue;

        status = umi_studio_runtime_surface_resolve(
            bindings->shell_registry,
            surface,
            &contribution);
        if (status != UMI_STATUS_OK) return status;

        status = umi_application_shell_registry_set_state(
            bindings->shell_registry,
            contribution.contribution_id,
            visible,
            (contribution.flags & UMI_APPLICATION_SHELL_ENABLED) != 0U,
            (contribution.flags & UMI_APPLICATION_SHELL_CHECKED) != 0U,
            contribution.badge_count);
        if (status != UMI_STATUS_OK) return status;

        status = umi_application_shell_layout_place(
            bindings->shell_layout,
            contribution.contribution_id,
            surface->container_id != NULL ? surface->container_id : "",
            surface->region,
            (size_t)(surface->order >= 0 ? surface->order : 0),
            visible);
        if (status != UMI_STATUS_OK) return status;
    }

    status = umi_application_shell_state_set_perspective(
        bindings->shell_state,
        preset->perspective_id);
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
