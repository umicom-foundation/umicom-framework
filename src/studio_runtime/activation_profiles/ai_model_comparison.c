/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/ai_model_comparison.c
 *
 * PURPOSE:
 *   Map the model comparison command to its dockable Studio surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/ai_model_comparison.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    {
        .command_pattern = "ai.developer.open-model-comparison",
        .surface = UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON,
        .prefix_match = 0,
        .make_visible = 1,
        .focus = 1
    }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_model_comparison(void)
{
    /* One exact rule avoids accidental activation by broad AI prefixes. */
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.ai-model-comparison",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
