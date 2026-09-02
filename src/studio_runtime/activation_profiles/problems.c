/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/problems.c
 *
 * PURPOSE:
 *   Define Problems command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/problems.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.problems", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.problems", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "navigate.problem", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "problems.", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile problems operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_problems(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.problems",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
