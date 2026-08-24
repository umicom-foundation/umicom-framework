/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/output.c
 *
 * PURPOSE:
 *   Define the Output Studio/Application Shell binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/output.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_output(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_OUTPUT,
        .ide_surface_id = "umicom.ide.output",
        .preferred_contribution_id = "umicom.shell.output.output",
        .fallback_contribution_id = "umicom.studio.surface.output",
        .activity_id = "",
        .container_id = "studio.bottom-panel",
        .title = "Output",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 110,
        .default_visible = 1,
        .create_if_missing = 1
    };
    return &binding;
}
