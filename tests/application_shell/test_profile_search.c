/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_search.c
 *
 * PURPOSE:
 *   Verify the built-in Search profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/profiles/search.h"

int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_search();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    assert(profile->contribution_count == 3U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.search.search",
        &contribution) == UMI_STATUS_OK);

    umi_application_shell_registry_destroy(registry);
    return 0;
}
