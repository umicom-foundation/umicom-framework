/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/ai_review.c
 *
 * PURPOSE:
 *   Define the AI Patch Review Studio/Application Shell binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/ai_review.h"

const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_ai_review(void)
{
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_AI_REVIEW,
        .ide_surface_id = "umicom.ai-developer.patch-review",
        .preferred_contribution_id = "",
        .fallback_contribution_id = "umicom.studio.surface.ai-review",
        .activity_id = "umicom.shell.activity-bar.ai",
        .container_id = "studio.editor",
        .title = "AI Patch Review",
        .role = UMI_APPLICATION_SHELL_ROLE_EDITOR,
        .region = UMI_APPLICATION_SHELL_REGION_EDITOR,
        .order = 200,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
