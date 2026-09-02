/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_preset_apply.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime layout preset apply.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/studio_runtime/layout_presets/zen.h"
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
    bindings=(UmiStudioRuntimeBindings){registry,&state,&layout,commands,
        (UmiIdeIntegrationPlatform*)1,(UmiDocumentCoordinator*)1,1U};
    assert(umi_studio_surface_catalogue_install(registry,&layout)==UMI_STATUS_OK);
    assert(umi_studio_layout_preset_apply(
        &bindings,umi_studio_layout_preset_zen())==UMI_STATUS_OK);
    assert(state.zen_mode);
    assert(!state.primary_sidebar_visible);
    assert(!state.bottom_panel_visible);
    umi_command_registry_destroy(commands);
    umi_application_shell_registry_destroy(registry);
    return 0;
}

