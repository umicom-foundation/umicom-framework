/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_problems.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Problems.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/problems.h"

int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_problems();

    assert(profile != NULL);
    assert(profile->rule_count == 4U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_PROBLEMS);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "view.problems"));
    return 0;
}
