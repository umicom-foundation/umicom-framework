/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/learning.c
 *
 * PURPOSE:
 *   Define the Guided Learning Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/learning.h"

/* Return immutable metadata that every frontend can discover by stable ID. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_learning(void)
{
    /* The panel exposes Framework Teacher content without copying curriculum
     * ownership into Studio or another client application. */
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_LEARNING,
        .ide_surface_id = "umicom.ide.learning",
        .preferred_contribution_id = "",
        .fallback_contribution_id = "umicom.studio.surface.learning",
        .activity_id = "umicom.shell.activity-bar.help",
        .container_id = "studio.secondary-sidebar",
        .title = "Guided Learning",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        .order = 210,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
