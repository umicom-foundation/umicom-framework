/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_builtin_profiles.c
 *
 * PURPOSE:
 *   Verify the complete reusable shell catalogue installs without duplicate IDs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/builtin_profiles.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationShellRegistry *registry = NULL;
    size_t index;

    assert(umi_application_shell_builtin_profile_count() >= 32U);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_shell_builtin_profile_count();
         ++index) {
        const UmiApplicationShellProfileDefinition *profile =
            umi_application_shell_builtin_profile_at(index);

        assert(profile != NULL);
        assert(umi_application_shell_profile_validate(profile) ==
               UMI_STATUS_OK);
        assert(umi_application_shell_builtin_profile_find(
            profile->profile_id) == profile);
    }

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_builtin_profiles_install(registry) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) > 100U);

    umi_application_shell_registry_destroy(registry);
    return 0;
}
