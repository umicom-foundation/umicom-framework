/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_preset_ai_development.c
 *
 * PURPOSE:
 *   Verify the AI Development Studio semantic layout preset.
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
#include "umicom/studio_runtime/layout_presets/ai_development.h"

int main(void)
{
    const UmiStudioRuntimeLayoutPresetDefinition *preset =
        umi_studio_layout_preset_ai_development();

    assert(preset != NULL);
    assert(strcmp(preset->preset_id, "umicom.studio.layout.ai-development") == 0);
    assert(preset->visible_surface_count == 5U);
    assert(umi_studio_layout_preset_validate(preset) == UMI_STATUS_OK);
    return 0;
}
