/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_build.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Build.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/build.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_build();

    assert(profile != NULL);
    assert(profile->rule_count == 6U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_BUILD);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "view.build"));
    return 0;
}
