/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/ai_tools.c
 *
 * PURPOSE:
 *   Define the AI Tool Activity Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/ai_tools.h"

/*
 * Provide the studio surface profile ai tools operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_ai_tools(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_AI_TOOLS,
        .ide_surface_id = "umicom.ai-developer.tool-activity",
        .preferred_contribution_id = "",
        .fallback_contribution_id = "umicom.studio.surface.ai-tools",
        .activity_id = "umicom.shell.activity-bar.ai",
        .container_id = "studio.bottom-panel",
        .title = "AI Tool Activity",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_BOTTOM_PANEL,
        .order = 140,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
