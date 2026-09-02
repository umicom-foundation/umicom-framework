/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_terminal.c
 *
 * PURPOSE:
 *   Verify the Terminal Studio surface binding contract.
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
#include "umicom/studio_runtime/surface_profiles/terminal.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_terminal();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_TERMINAL);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.terminal") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.terminal") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
