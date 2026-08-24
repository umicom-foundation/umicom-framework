/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_debug.c
 *
 * PURPOSE:
 *   Verify the Run and Debug Studio surface binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/surface_profiles/debug.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_debug();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_DEBUG);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.debug") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.debug") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
