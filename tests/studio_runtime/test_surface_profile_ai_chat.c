/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_surface_profile_ai_chat.c
 *
 * PURPOSE:
 *   Verify the AI Coding Chat Studio surface binding contract.
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
#include "umicom/studio_runtime/surface_profiles/ai_chat.h"

int main(void)
{
    const UmiStudioRuntimeSurfaceBinding *binding =
        umi_studio_surface_profile_ai_chat();

    assert(binding != NULL);
    assert(binding->kind == UMI_STUDIO_SURFACE_AI_CHAT);
    assert(strcmp(binding->ide_surface_id, "umicom.ai-developer.chat") == 0);
    assert(strcmp(binding->fallback_contribution_id, "umicom.studio.surface.ai-chat") == 0);
    assert(umi_studio_runtime_surface_binding_validate(binding) ==
           UMI_STATUS_OK);
    return 0;
}
