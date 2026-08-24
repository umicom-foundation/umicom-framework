/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/test_explorer.c
 *
 * PURPOSE:
 *   Define Test Explorer command activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/test_explorer.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.test-explorer", .surface = UMI_STUDIO_SURFACE_TEST_EXPLORER, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.test-explorer", .surface = UMI_STUDIO_SURFACE_TEST_EXPLORER, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "test.", .surface = UMI_STUDIO_SURFACE_TEST_EXPLORER, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_test_explorer(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.test-explorer",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
