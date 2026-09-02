/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/terminal.c
 *
 * PURPOSE:
 *   Define the Terminal Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/terminal.h"

/*
 * Provide the studio surface profile terminal operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_terminal(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_TERMINAL,
        .ide_surface_id = "umicom.ide.terminal",
        .preferred_contribution_id = "umicom.shell.terminal.terminal",
        .fallback_contribution_id = "umicom.studio.surface.terminal",
        .activity_id = "",
        .container_id = "studio.bottom-panel",
        .title = "Terminal",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 120,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
