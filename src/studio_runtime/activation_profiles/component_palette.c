/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/component_palette.c
 * PURPOSE: Route component discovery commands to the palette surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/component_palette.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.palette", .surface = UMI_STUDIO_SURFACE_COMPONENT_PALETTE, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "view.component-palette", .surface = UMI_STUDIO_SURFACE_COMPONENT_PALETTE, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for component discovery. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_component_palette(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.component-palette",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
