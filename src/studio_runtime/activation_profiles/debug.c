/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/debug.c
 *
 * PURPOSE:
 *   Define Run and Debug command activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/debug.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.debug", .surface = UMI_STUDIO_SURFACE_DEBUG, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.debug", .surface = UMI_STUDIO_SURFACE_DEBUG, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "debug.", .surface = UMI_STUDIO_SURFACE_DEBUG, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_debug(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.debug",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
