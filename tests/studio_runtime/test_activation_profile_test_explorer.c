/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_test_explorer.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Test Explorer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/test_explorer.h"

int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_test_explorer();

    assert(profile != NULL);
    assert(profile->rule_count == 3U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_TEST_EXPLORER);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "view.test-explorer"));
    return 0;
}
