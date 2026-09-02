/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/visual_designer.c
 * PURPOSE: Route visual-design commands to the canvas surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/visual_designer.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.open", .surface = UMI_STUDIO_SURFACE_VISUAL_DESIGNER, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "designer.canvas", .surface = UMI_STUDIO_SURFACE_VISUAL_DESIGNER, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "studio.layout.visual-design", .surface = UMI_STUDIO_SURFACE_VISUAL_DESIGNER, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for the designer canvas. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_visual_designer(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.visual-designer",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
