/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/search.c
 *
 * PURPOSE:
 *   Define Search command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/search.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "view.search", .surface = UMI_STUDIO_SURFACE_SEARCH, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "workbench.view.search", .surface = UMI_STUDIO_SURFACE_SEARCH, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "search.", .surface = UMI_STUDIO_SURFACE_SEARCH, .prefix_match = 1, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile search operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_search(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.search",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
