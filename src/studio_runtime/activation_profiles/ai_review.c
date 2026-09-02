/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/ai_review.c
 *
 * PURPOSE:
 *   Define AI Patch Review command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/ai_review.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "ide.ai.open-review", .surface = UMI_STUDIO_SURFACE_AI_REVIEW, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "ai.developer.open-review", .surface = UMI_STUDIO_SURFACE_AI_REVIEW, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "ai.developer.patch-", .surface = UMI_STUDIO_SURFACE_AI_REVIEW, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile ai review operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_review(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.ai-review",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
