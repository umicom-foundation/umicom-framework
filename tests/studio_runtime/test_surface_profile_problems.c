/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_problems.c
 *
 * PURPOSE:
 *   Verify the Problems Studio surface binding contract.
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
#include "umicom/studio_runtime/surface_profiles/problems.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_problems();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_PROBLEMS);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.problems") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.problems") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
