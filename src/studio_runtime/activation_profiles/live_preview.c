/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/live_preview.c
 *
 * PURPOSE:
 *   Route the shared View command to the active-document preview panel.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/live_preview.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    {
        .command_pattern = "view.live-preview",
        .surface = UMI_STUDIO_SURFACE_LIVE_PREVIEW,
        .prefix_match = 0,
        .make_visible = 1,
        .focus = 1
    }
};

/* Publish one exact rule so broad View routes cannot shadow this panel. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_live_preview(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.live-preview",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
