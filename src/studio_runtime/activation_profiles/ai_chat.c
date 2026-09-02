/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/ai_chat.c
 *
 * PURPOSE:
 *   Define AI Coding Chat command activation behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/ai_chat.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "ai.assistant.open", .surface = UMI_STUDIO_SURFACE_AI_CHAT, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "ide.ai.open-overview", .surface = UMI_STUDIO_SURFACE_AI_CHAT, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "ai.developer.new-chat", .surface = UMI_STUDIO_SURFACE_AI_CHAT, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "ai.developer.open-overview", .surface = UMI_STUDIO_SURFACE_AI_CHAT, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

/*
 * Provide the studio activation profile ai chat operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_chat(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.ai-chat",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
