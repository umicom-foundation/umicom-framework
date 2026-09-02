/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_shell/test_persona.c
 *
 * PURPOSE:
 *   Verify Studio, Desk and generic thin-application personas resolve and
 *   install only Framework-owned profiles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application_shell/persona.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationShellRegistry *registry = NULL;
    const UmiApplicationShellPersonaDefinition *studio;
    const UmiApplicationShellPersonaDefinition *desk;
    size_t index;

    assert(umi_application_shell_persona_count() >= 3U);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_application_shell_persona_count(); ++index) {
        assert(umi_application_shell_persona_validate(
            umi_application_shell_persona_at(index)) == UMI_STATUS_OK);
    }

    studio = umi_application_shell_persona_find(
        "umicom.shell.persona.studio-ide");
    desk = umi_application_shell_persona_find(
        "umicom.shell.persona.desk");

    assert(studio != NULL);
    assert(desk != NULL);
    assert(studio->profile_count > desk->profile_count);

    assert(umi_application_shell_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_application_shell_persona_install(
        registry, studio) == UMI_STATUS_OK);
    assert(umi_application_shell_registry_count(registry) > 100U);
    umi_application_shell_registry_destroy(registry);

    return 0;
}
