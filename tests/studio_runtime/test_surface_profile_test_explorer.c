/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_test_explorer.c
 *
 * PURPOSE:
 *   Verify the Test Explorer Studio surface binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/surface_profiles/test_explorer.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_test_explorer();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_TEST_EXPLORER);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.test-explorer") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.test-explorer") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
