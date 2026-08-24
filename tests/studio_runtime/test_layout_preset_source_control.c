/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_preset_source_control.c
 *
 * PURPOSE:
 *   Verify the Source Control Studio semantic layout preset.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/layout_presets/source_control.h"

int main(void)
{
    const UmiStudioRuntimeLayoutPresetDefinition *preset =
        umi_studio_layout_preset_source_control();

    assert(preset != NULL);
    assert(strcmp(preset->preset_id, "umicom.studio.layout.source-control") == 0);
    assert(preset->visible_surface_count == 4U);
    assert(umi_studio_layout_preset_validate(preset) == UMI_STATUS_OK);
    return 0;
}
