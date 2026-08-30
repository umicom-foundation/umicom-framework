/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/test_explorer.c
 *
 * PURPOSE:
 *   Define the Test Explorer Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/test_explorer.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_test_explorer(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_TEST_EXPLORER,
        .ide_surface_id = "umicom.ide.test-explorer",
        .preferred_contribution_id = "umicom.shell.test-explorer.test-explorer",
        .fallback_contribution_id = "umicom.studio.surface.test-explorer",
        .activity_id = "umicom.shell.activity-bar.testing",
        .container_id = "studio.primary-sidebar",
        .title = "Test Explorer",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_PRIMARY_SIDEBAR,
        .order = 130,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
