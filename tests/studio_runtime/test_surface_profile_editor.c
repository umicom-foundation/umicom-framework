/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_editor.c
 *
 * PURPOSE:
 *   Verify the Code Editor Studio surface binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/surface_profiles/editor.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_editor();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_EDITOR);
    assert(strcmp(binding->ide_surface_id, "umicom.ide.editor") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.editor") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
