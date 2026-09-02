/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/object_inspector.c
 * PURPOSE: Route property-editing commands to the object inspector surface.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/object_inspector.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "designer.inspector", .surface = UMI_STUDIO_SURFACE_OBJECT_INSPECTOR, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "designer.property", .surface = UMI_STUDIO_SURFACE_OBJECT_INSPECTOR, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "view.object-inspector", .surface = UMI_STUDIO_SURFACE_OBJECT_INSPECTOR, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/* Publish the immutable command routing rules for property inspection. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_object_inspector(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.object-inspector",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
