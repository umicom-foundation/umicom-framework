/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/outline.c
 *
 * PURPOSE:
 *   Define Outline command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/outline.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.outline", .surface = UMI_STUDIO_SURFACE_OUTLINE, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.outline", .surface = UMI_STUDIO_SURFACE_OUTLINE, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile outline operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_outline(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.outline",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
