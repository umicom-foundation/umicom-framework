/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/explorer.c
 *
 * PURPOSE:
 *   Define Explorer command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/explorer.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.explorer", .surface = UMI_STUDIO_SURFACE_EXPLORER, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.explorer", .surface = UMI_STUDIO_SURFACE_EXPLORER, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "project.", .surface = UMI_STUDIO_SURFACE_EXPLORER, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workspace.", .surface = UMI_STUDIO_SURFACE_EXPLORER, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile explorer operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_explorer(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.explorer",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
