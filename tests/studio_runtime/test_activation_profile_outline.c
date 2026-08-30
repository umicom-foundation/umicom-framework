/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_outline.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Outline.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/outline.h"

int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_outline();

    assert(profile != NULL);
    assert(profile->rule_count == 2U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_OUTLINE);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "view.outline"));
    return 0;
}
