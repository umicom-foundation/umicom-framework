/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/ai_tools.c
 *
 * PURPOSE:
 *   Define AI Tool Activity command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/ai_tools.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "ai.developer.open-tools", .surface = UMI_STUDIO_SURFACE_AI_TOOLS, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_tools(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.ai-tools",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
