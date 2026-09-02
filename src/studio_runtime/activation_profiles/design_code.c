/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/design_code.c
 * PURPOSE: Route design-source commands to the Code surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/design_code.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.code", .surface = UMI_STUDIO_SURFACE_DESIGN_CODE, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for the Code surface. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_design_code(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.design-code",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
