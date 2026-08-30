/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_editor.c
 *
 * PURPOSE:
 *   Verify the built-in Editor profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/profiles/editor.h"

int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_editor();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    assert(profile->contribution_count == 4U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.editor.code-editor",
        &contribution) == UMI_STATUS_OK);

    umi_application_shell_registry_destroy(registry);
    return 0;
}
