/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_file_menu.c
 *
 * PURPOSE:
 *   Verify the built-in File Menu profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Assertions here are test checks and must also run in Release. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include "umicom/application_shell/profiles/file_menu.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_file_menu();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    /* Previous profile: assert(profile->contribution_count == 11U);
     * Retain its eleven entries and add the shared Cancel Save All action. */
    assert(profile->contribution_count == 12U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.file-menu.root",
        &contribution) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_find(registry,
        "umicom.shell.file-menu.cancel-save-all", &contribution) == UMI_STATUS_OK);
    assert(strcmp(contribution.command_id, "file.save-all.cancel") == 0);
    assert(umi_application_shell_registry_find(registry,
        "umicom.shell.file-menu.save-all", &contribution) == UMI_STATUS_OK);
    assert(strcmp(contribution.command_id, "file.save-all") == 0);
    umi_application_shell_registry_destroy(registry);
    return 0;
}
