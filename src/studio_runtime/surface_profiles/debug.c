/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/debug.c
 *
 * PURPOSE:
 *   Define the Run and Debug Studio/Application Shell binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/debug.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_debug(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_DEBUG,
        .ide_surface_id = "umicom.ide.debug",
        .preferred_contribution_id = "umicom.shell.debug.debug",
        .fallback_contribution_id = "umicom.studio.surface.debug",
        .activity_id = "umicom.shell.activity-bar.run-debug",
        .container_id = "studio.primary-sidebar",
        .title = "Run and Debug",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 140,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
