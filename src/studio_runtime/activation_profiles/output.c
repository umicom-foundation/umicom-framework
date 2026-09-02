/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/output.c
 *
 * PURPOSE:
 *   Define Output command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/output.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.output", .surface = UMI_STUDIO_SURFACE_OUTPUT, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.output", .surface = UMI_STUDIO_SURFACE_OUTPUT, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "output.", .surface = UMI_STUDIO_SURFACE_OUTPUT, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile output operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_output(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.output",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
