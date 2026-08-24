/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/source_control.c
 *
 * PURPOSE:
 *   Define Source Control command activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/source_control.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.source-control", .surface = UMI_STUDIO_SURFACE_SOURCE_CONTROL, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.source-control", .surface = UMI_STUDIO_SURFACE_SOURCE_CONTROL, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "source-control.", .surface = UMI_STUDIO_SURFACE_SOURCE_CONTROL, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_source_control(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.source-control",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
