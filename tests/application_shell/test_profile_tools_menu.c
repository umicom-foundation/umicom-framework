/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_tools_menu.c
 *
 * PURPOSE:
 *   Verify the built-in Tools Menu profile is valid and installable.
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

#include "umicom/application_shell/profiles/tools_menu.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_tools_menu();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    /* Keep the prior catalogue size beside the added cancellation contract. */
#if 0
    assert(profile->contribution_count == 7U);
#endif
    assert(profile->contribution_count == 8U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.tools-menu.root",
        &contribution) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_find(registry,
        "umicom.shell.tools-menu.codeguard-cancel", &contribution) == UMI_STATUS_OK);
    assert(strcmp(contribution.command_id, "codeguard.cancel") == 0);
    assert(strcmp(contribution.group_id, "quality") == 0);
    assert(contribution.order == 310);
    umi_application_shell_registry_destroy(registry);
    return 0;
}
