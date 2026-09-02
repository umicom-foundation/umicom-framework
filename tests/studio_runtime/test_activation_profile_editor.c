/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_activation_profile_editor.c
 *
 * PURPOSE:
 *   Verify command activation mapping for Code Editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/studio_runtime/activation_profiles/editor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeActivationProfile *profile =
        umi_studio_activation_profile_editor();

    assert(profile != NULL);
    assert(profile->rule_count == 6U);
    assert(profile->rules[0].surface == UMI_STUDIO_SURFACE_EDITOR);
    assert(umi_studio_activation_rule_matches(
        &profile->rules[0],
        "ide.open..sample"));
    return 0;
}
