/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/outline.c
 *
 * PURPOSE:
 *   Define the Outline Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/outline.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_outline(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_OUTLINE,
        .ide_surface_id = "umicom.ide.outline",
        .preferred_contribution_id = "",
        .fallback_contribution_id = "umicom.studio.surface.outline",
        .activity_id = "",
        .container_id = "studio.secondary-sidebar",
        .title = "Outline",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        .order = 120,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
