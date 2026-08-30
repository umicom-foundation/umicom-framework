/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_build.c
 *
 * PURPOSE:
 *   Verify the Build Studio surface binding contract.
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
#include "umicom/studio_runtime/surface_profiles/build.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_build();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_BUILD);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.build") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.build") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
