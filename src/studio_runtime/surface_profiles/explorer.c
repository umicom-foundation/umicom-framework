/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/explorer.c
 *
 * PURPOSE:
 *   Define the Explorer Studio/Application Shell binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/explorer.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_explorer(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_EXPLORER,
        .ide_surface_id = "umicom.ide.explorer",
        .preferred_contribution_id = "umicom.shell.project-explorer.explorer",
        .fallback_contribution_id = "umicom.studio.surface.explorer",
        .activity_id = "umicom.shell.activity-bar.explorer",
        .container_id = "studio.primary-sidebar",
        .title = "Explorer",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 100,
        .default_visible = 1,
        .create_if_missing = 1
    };
    return &binding;
}
