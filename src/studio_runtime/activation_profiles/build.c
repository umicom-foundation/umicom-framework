/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/build.c
 *
 * PURPOSE:
 *   Define Build command activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/build.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.build", .surface = UMI_STUDIO_SURFACE_BUILD, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.build", .surface = UMI_STUDIO_SURFACE_BUILD, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "build.", .surface = UMI_STUDIO_SURFACE_BUILD, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "developer.build", .surface = UMI_STUDIO_SURFACE_BUILD, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "developer.configure", .surface = UMI_STUDIO_SURFACE_BUILD, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "developer.package", .surface = UMI_STUDIO_SURFACE_BUILD, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_build(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.build",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
