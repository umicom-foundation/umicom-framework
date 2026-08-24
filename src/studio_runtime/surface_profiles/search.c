/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/search.c
 *
 * PURPOSE:
 *   Define the Search Studio/Application Shell binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/search.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_search(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_SEARCH,
        .ide_surface_id = "umicom.ide.search",
        .preferred_contribution_id = "umicom.shell.search.search",
        .fallback_contribution_id = "umicom.studio.surface.search",
        .activity_id = "umicom.shell.activity-bar.search",
        .container_id = "studio.primary-sidebar",
        .title = "Search",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 110,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
