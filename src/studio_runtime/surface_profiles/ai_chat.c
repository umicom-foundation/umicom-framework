/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/ai_chat.c
 *
 * PURPOSE:
 *   Define the AI Coding Chat Studio/Application Shell binding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/ai_chat.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_ai_chat(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_AI_CHAT,
        .ide_surface_id = "umicom.ai-developer.chat",
        .preferred_contribution_id = "",
        .fallback_contribution_id = "umicom.studio.surface.ai-chat",
        .activity_id = "umicom.shell.activity-bar.ai",
        .container_id = "studio.secondary-sidebar",
        .title = "AI Coding Chat",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        .order = 100,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
