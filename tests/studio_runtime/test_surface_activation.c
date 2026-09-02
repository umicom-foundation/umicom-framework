/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_activation.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime surface activation.
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
#include "umicom/studio_runtime/surface_activator.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationShellRegistry *registry=NULL;
    UmiApplicationShellState state;
    UmiApplicationShellLayout layout;
    UmiCommandRegistry *commands=NULL;
    UmiStudioRuntimeBindings bindings;
    assert(umi_application_shell_registry_create(&registry)==UMI_STATUS_OK);
    assert(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    umi_application_shell_state_init(&state);
    umi_application_shell_layout_init(&layout,"layout","Layout");
    umi_studio_runtime_bindings_init(&bindings);
    bindings.shell_registry=registry; bindings.shell_state=&state;
    bindings.shell_layout=&layout; bindings.commands=commands;
    bindings.ide=(UmiIdeIntegrationPlatform*)1;
    bindings.documents=(UmiDocumentCoordinator*)1;
    assert(umi_studio_surface_catalogue_install(registry,&layout)==UMI_STATUS_OK);
    assert(umi_studio_surface_activate_kind(
        &bindings,UMI_STUDIO_SURFACE_PROBLEMS,1,1)==UMI_STATUS_OK);
    assert(state.active_view_id[0]!='\0');
    assert(strcmp(state.active_view_id,state.focused_view_id)==0);
    umi_command_registry_destroy(commands);
    umi_application_shell_registry_destroy(registry);
    return 0;
}

