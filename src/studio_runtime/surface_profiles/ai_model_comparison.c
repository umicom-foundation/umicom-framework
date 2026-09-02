/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/surface_profiles/ai_model_comparison.c
 *
 * PURPOSE:
 *   Define the Framework-owned model comparison surface placement contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/surface_profiles/ai_model_comparison.h"

/*
 * Provide the studio surface profile ai model comparison operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_ai_model_comparison(void)
{
    /* The immutable binding can be shared safely by every Studio host. */
    static const UmiStudioRuntimeSurfaceBinding binding = {
        .kind = UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON,
        .ide_surface_id = "umicom.ai.model-comparison",
        .preferred_contribution_id = "",
        .fallback_contribution_id =
            "umicom.studio.surface.ai-model-comparison",
        .activity_id = "umicom.shell.activity-bar.ai",
        .container_id = "studio.secondary-sidebar",
        .title = "Model Comparison",
        .role = UMI_APPLICATION_SHELL_ROLE_PANEL,
        .region = UMI_APPLICATION_SHELL_REGION_SECONDARY_SIDEBAR,
        .order = 105,
        .default_visible = 0,
        .create_if_missing = 1
    };
    return &binding;
}
