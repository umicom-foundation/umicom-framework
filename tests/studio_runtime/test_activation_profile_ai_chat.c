/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_ai_chat.c
 *
 * PURPOSE:
 *   Verify command activation mapping for AI Coding Chat.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/ai_chat.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_ai_chat();

    assert(profile != NULL);
    assert(profile->rule_count == 4U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_AI_CHAT);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "ai.assistant.open"));
    return 0;
}
