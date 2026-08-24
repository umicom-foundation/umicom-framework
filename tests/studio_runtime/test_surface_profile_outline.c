/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_outline.c
 *
 * PURPOSE:
 *   Verify the Outline Studio surface binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/surface_profiles/outline.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_outline();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_OUTLINE);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.outline") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.outline") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
