/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/source_control.c
 *
 * PURPOSE:
 *   Define the Source Control Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/source_control.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_source_control(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_SOURCE_CONTROL,
        .ide_surface_id = "umicom.ide.source-control",
        .preferred_contribution_id = "umicom.shell.source-control.source-control",
        .fallback_contribution_id = "umicom.studio.surface.source-control",
        .activity_id = "umicom.shell.activity-bar.source-control",
        .container_id = "studio.primary-sidebar",
        .title = "Source Control",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 120,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
