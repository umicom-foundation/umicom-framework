/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_explorer.c
 *
 * PURPOSE:
 *   Verify the Explorer Studio surface binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/surface_profiles/explorer.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_explorer();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_EXPLORER);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.explorer") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.explorer") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
