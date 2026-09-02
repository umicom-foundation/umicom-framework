/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/ai_approvals.c
 *
 * PURPOSE:
 *   Define AI Approvals command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/ai_approvals.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "ai.developer.open-approvals", .surface = UMI_STUDIO_SURFACE_AI_APPROVALS, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "ai.developer.approval-", .surface = UMI_STUDIO_SURFACE_AI_APPROVALS, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile ai approvals operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_approvals(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.ai-approvals",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
