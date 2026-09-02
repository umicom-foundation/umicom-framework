/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/design_mixed.c
 * PURPOSE: Route split authoring commands to the Mixed surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/design_mixed.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.mixed", .surface = UMI_STUDIO_SURFACE_DESIGN_MIXED, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for the Mixed surface. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_design_mixed(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.design-mixed",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
