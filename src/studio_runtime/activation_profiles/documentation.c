/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/documentation.c
 *
 * PURPOSE:
 *   Map documentation, browser and preview commands to one reusable surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/documentation.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "help.documentation", .surface = UMI_STUDIO_SURFACE_DOCUMENTATION, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "view.documentation", .surface = UMI_STUDIO_SURFACE_DOCUMENTATION, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "documentation.", .surface = UMI_STUDIO_SURFACE_DOCUMENTATION, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "browser.preview", .surface = UMI_STUDIO_SURFACE_DOCUMENTATION, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the bounded rule array without transferring its storage to callers. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_documentation(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.documentation",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
