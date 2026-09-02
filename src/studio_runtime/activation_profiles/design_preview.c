/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/design_preview.c
 * PURPOSE: Route live-preview commands to the Preview surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/design_preview.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.preview", .surface = UMI_STUDIO_SURFACE_DESIGN_PREVIEW, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for the Preview surface. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_design_preview(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.design-preview",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
