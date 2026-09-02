/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_source_control.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Source Control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/source_control.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_source_control();

    assert(profile != NULL);
    assert(profile->rule_count == 3U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_SOURCE_CONTROL);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "view.source-control"));
    return 0;
}
