/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/build.c
 *
 * PURPOSE:
 *   Define the Build Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/build.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_build(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_BUILD,
        .ide_surface_id = "umicom.ide.build",
        .preferred_contribution_id = "umicom.shell.build-dashboard.build-dashboard",
        .fallback_contribution_id = "umicom.studio.surface.build",
        .activity_id = "",
        .container_id = "studio.bottom-panel",
        .title = "Build",
        .role = UMI_APPLICATION_SHELL_ROLE_DASHBOARD,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 130,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
