/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/component_structure.c
 * PURPOSE: Route component hierarchy commands to the structure surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/component_structure.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.structure", .surface = UMI_STUDIO_SURFACE_COMPONENT_STRUCTURE, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "view.component-structure", .surface = UMI_STUDIO_SURFACE_COMPONENT_STRUCTURE, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for component hierarchy work. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_component_structure(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.component-structure",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
