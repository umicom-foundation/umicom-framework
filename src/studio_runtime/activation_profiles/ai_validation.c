/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/ai_validation.c
 *
 * PURPOSE:
 *   Define AI Validation command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/ai_validation.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "ai.developer.open-validation", .surface = UMI_STUDIO_SURFACE_AI_VALIDATION, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_validation(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.ai-validation",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
