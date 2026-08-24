/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_ai_tools.c
 *
 * PURPOSE:
 *   Verify the AI Tool Activity Studio surface binding contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/surface_profiles/ai_tools.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_ai_tools();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_AI_TOOLS);
    assert(strcmp(binding->ide_surface_id, "umicom.ai-developer.tool-activity") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.ai-tools") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
