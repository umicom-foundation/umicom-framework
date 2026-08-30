/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/terminal.c
 *
 * PURPOSE:
 *   Define Terminal command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/terminal.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.terminal", .surface = UMI_STUDIO_SURFACE_TERMINAL, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.terminal", .surface = UMI_STUDIO_SURFACE_TERMINAL, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "terminal.", .surface = UMI_STUDIO_SURFACE_TERMINAL, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_terminal(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.terminal",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
