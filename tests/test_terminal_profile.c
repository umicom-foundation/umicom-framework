/*-----------------------------------------------------------------------------
 * Umicom Framework terminal-profile tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/terminal/profile_registry.h"

int main(void)
{
    UmiTerminalProfile profile;
    UmiTerminalProfile copy;
    UmiTerminalProfileRegistry *registry = NULL;
    char command[512];

    umi_terminal_profile_init(&profile);
    (void)snprintf(profile.profile_id, sizeof(profile.profile_id), "%s", "test-shell");
    (void)snprintf(profile.title, sizeof(profile.title), "%s", "Test Shell");
    (void)snprintf(profile.program, sizeof(profile.program), "%s", "/bin/sh");
    profile.kind = UMI_TERMINAL_PROFILE_POSIX_SH;
    assert(umi_terminal_profile_set_argument(&profile, 0U, "-l") == UMI_STATUS_OK);
    assert(umi_terminal_profile_set_environment(&profile, "UMICOM_MODE", "test") ==
           UMI_STATUS_OK);
    assert(umi_terminal_profile_validate(&profile) == UMI_STATUS_OK);
    assert(umi_terminal_profile_build_command(&profile, command, sizeof(command)) ==
           UMI_STATUS_OK);
    assert(strcmp(command, "\"/bin/sh\" \"-l\"") == 0);

    assert(umi_terminal_profile_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_terminal_profile_registry_register(registry, &profile) == UMI_STATUS_OK);
    assert(umi_terminal_profile_registry_count(registry) == 1U);
    assert(strcmp(umi_terminal_profile_registry_default_id(registry), "test-shell") == 0);
    assert(umi_terminal_profile_registry_find(registry, "test-shell", &copy) ==
           UMI_STATUS_OK);
    assert(strcmp(copy.environment[0].value, "test") == 0);
    assert(umi_terminal_profile_registry_remove(registry, "test-shell") == UMI_STATUS_OK);
    assert(umi_terminal_profile_registry_count(registry) == 0U);
    umi_terminal_profile_registry_destroy(registry);
    return 0;
}
