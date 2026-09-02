/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/documentation.c
 *
 * PURPOSE:
 *   Define the Documentation Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/documentation.h"

/* Return immutable metadata that every frontend can discover by stable ID. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_documentation(void)
{
    /* Applications may replace the fallback contribution with their own thin
     * renderer while preserving the same Framework surface identity. */
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_DOCUMENTATION,
        .ide_surface_id = "umicom.ide.documentation",
        .preferred_contribution_id = "",
        .fallback_contribution_id = "umicom.studio.surface.documentation",
        .activity_id = "umicom.shell.activity-bar.help",
        .container_id = "studio.secondary-sidebar",
        .title = "Documentation",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        .order = 220,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
