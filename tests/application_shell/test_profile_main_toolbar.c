/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_main_toolbar.c
 *
 * PURPOSE:
 *   Verify the built-in Main Toolbar profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/profiles/main_toolbar.h"

int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_main_toolbar();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    assert(profile->contribution_count == 9U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.main-toolbar.new",
        &contribution) == UMI_STATUS_OK);

    umi_application_shell_registry_destroy(registry);
    return 0;
}
