/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/learning.c
 *
 * PURPOSE:
 *   Map learning and Teacher commands to the guided-learning surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/learning.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "help.ai-teacher", .surface = UMI_STUDIO_SURFACE_LEARNING, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "view.learning", .surface = UMI_STUDIO_SURFACE_LEARNING, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "learning.", .surface = UMI_STUDIO_SURFACE_LEARNING, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "teacher.", .surface = UMI_STUDIO_SURFACE_LEARNING, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/* Publish the bounded rule array without transferring its storage to callers. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_learning(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.learning",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
