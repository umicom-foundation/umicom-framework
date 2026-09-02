/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_ai_tools.c
 *
 * PURPOSE:
 *   Verify command activation mapping for AI Tool Activity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/ai_tools.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_ai_tools();

    assert(profile != NULL);
    assert(profile->rule_count == 1U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_AI_TOOLS);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "ai.developer.open-tools"));
    return 0;
}
