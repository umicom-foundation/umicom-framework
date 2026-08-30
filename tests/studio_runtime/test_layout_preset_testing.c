/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_preset_testing.c
 *
 * PURPOSE:
 *   Verify the Testing Studio semantic layout preset.
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
#include "umicom/studio_runtime/layout_presets/testing.h"

int main(void)
{
    const UmiStudioRuntimeLayoutPresetDefinition *preset =
        umi_studio_layout_preset_testing();

    assert(preset != NULL);
    assert(strcmp(preset->preset_id, "umicom.studio.layout.testing") == 0);
    assert(preset->visible_surface_count == 4U);
    assert(umi_studio_layout_preset_validate(preset) == UMI_STATUS_OK);
    return 0;
}
