/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_profile_edit_menu.c
 *
 * PURPOSE:
 *   Verify the built-in Edit Menu profile is valid and installable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifdef NDEBUG
#undef NDEBUG /* Keep acceptance assertions active in Release builds too. */
#endif
#include <assert.h>
#include <string.h>

#include "umicom/application_shell/profiles/edit_menu.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiApplicationShellProfileDefinition *profile =
        umi_application_shell_profile_edit_menu();
    UmiApplicationShellRegistry *registry = NULL;
    UmiApplicationShellContribution contribution;

    assert(profile != NULL);
    assert(profile->contribution_count == 11U);
    assert(umi_application_shell_profile_validate(profile) == UMI_STATUS_OK);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_profile_install(registry, profile) ==
           UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) ==
           profile->contribution_count);
    assert(umi_application_shell_registry_find(
        registry,
        "umicom.shell.edit-menu.root",
        &contribution) == UMI_STATUS_OK);

    /* Adding selection commands must retain every existing editing/search route. */
    static const char *required[] = {
        "edit.undo", "edit.redo", "edit.cut", "edit.copy", "edit.paste",
        "edit.select-all", "edit.delete", "search.find", "search.replace", "search.workspace"
    };
    for (size_t index = 0U; index < sizeof(required) / sizeof(required[0]); ++index) {
        size_t matches = 0U;
        for (size_t item = 0U; item < profile->contribution_count; ++item)
            if (strcmp(profile->contributions[item].command_id, required[index]) == 0) ++matches;
        assert(matches == 1U);
    }

    umi_application_shell_registry_destroy(registry);
    return 0;
}
