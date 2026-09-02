/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_explorer.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Explorer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/explorer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_explorer();

    assert(profile != NULL);
    assert(profile->rule_count == 4U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_EXPLORER);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "view.explorer"));
    return 0;
}
