/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_help_menu.c
 *
 * PURPOSE:
 *   Verify the built-in Help Menu profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>

#include "umicom/application_shell/profiles/help_menu.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_help_menu();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    /* The earlier six entries remain; the verified user guide adds one. */
#if 0
    assert(profile->contribution_count == 6U);
#endif
    assert(profile->contribution_count == 7U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.help-menu.root",
        &contribution) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_find(registry,
        "umicom.shell.help-menu.user-guide", &contribution) == UMI_STATUS_OK);
    assert(strcmp(contribution.command_id, "help.learning.user-guide") == 0);
    assert(contribution.role == UMI_APPLICATION_SHELL_ROLE_MENU_ITEM);
    umi_application_shell_registry_destroy(registry);
    return 0;
}
