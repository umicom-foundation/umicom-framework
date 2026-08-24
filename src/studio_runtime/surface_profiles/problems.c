/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/problems.c
 *
 * PURPOSE:
 *   Define the Problems Studio/Application Shell binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/problems.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_problems(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_PROBLEMS,
        .ide_surface_id = "umicom.ide.problems",
        .preferred_contribution_id = "umicom.shell.problems.problems",
        .fallback_contribution_id = "umicom.studio.surface.problems",
        .activity_id = "",
        .container_id = "studio.bottom-panel",
        .title = "Problems",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 100,
        .default_visible = 1,
        .create_if_missing = 1
    };
    return &binding;
}
