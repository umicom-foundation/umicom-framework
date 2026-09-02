/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_view_menu.c
 *
 * PURPOSE:
 *   Verify the built-in View Menu profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application_shell/profiles/view_menu.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_view_menu();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    assert(profile->contribution_count == 12U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.view-menu.root",
        &contribution) == UMI_STATUS_OK);
    /* The command is public through the normal View menu profile. */
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.view-menu.live-preview",
        &contribution) == UMI_STATUS_OK);
    assert(strcmp(contribution.command_id, "view.live-preview") == 0);

    umi_application_shell_registry_destroy(registry);
    return 0;
}
