/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_ai_approvals.c
 *
 * PURPOSE:
 *   Verify command activation mapping for AI Approvals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/ai_approvals.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_ai_approvals();

    assert(profile != NULL);
    assert(profile->rule_count == 2U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_AI_APPROVALS);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "ai.developer.open-approvals"));
    return 0;
}
