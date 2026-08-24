/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_alias_registry.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime command alias registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/studio_runtime/command_alias_registry.h"
int main(void)
{
    UmiApplicationShellRegistry *shell=NULL;
    UmiApplicationShellState state;
    UmiApplicationShellLayout layout;
    UmiCommandRegistry *commands=NULL;
    UmiStudioRuntimeBindings bindings;
    UmiStudioRuntimeCommandAliasRegistry *aliases=NULL;
    assert(umi_application_shell_registry_create(&shell)==UMI_STATUS_OK);
    assert(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    umi_application_shell_state_init(&state);
    umi_application_shell_layout_init(&layout,"layout","Layout");
    bindings=(UmiStudioRuntimeBindings){shell,&state,&layout,commands,
        (UmiIdeIntegrationPlatform*)1,(UmiDocumentCoordinator*)1,1U};
    assert(umi_studio_surface_catalogue_install(shell,&layout)==UMI_STATUS_OK);
    assert(umi_studio_command_alias_registry_create(
        &bindings,&aliases)==UMI_STATUS_OK);
    assert(umi_studio_command_alias_registry_install(aliases)==UMI_STATUS_OK);
    assert(umi_command_registry_contains(commands,"view.explorer"));
    assert(umi_studio_command_alias_registry_registered_count(aliases)>0U);
    umi_studio_command_alias_registry_destroy(aliases);
    umi_command_registry_destroy(commands);
    umi_application_shell_registry_destroy(shell);
    return 0;
}

