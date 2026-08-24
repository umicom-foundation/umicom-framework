/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/problems.c
 *
 * PURPOSE:
 *   Define Problems command activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/problems.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.problems", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.problems", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "navigate.problem", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "problems.", .surface = UMI_STUDIO_SURFACE_PROBLEMS, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

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
