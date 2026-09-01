/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_layout_preset_debugging.c
 *
 * PURPOSE:
 *   Verify the Debugging Studio semantic layout preset.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>
#include "umicom/studio_runtime/layout_presets/debugging.h"
#include "umicom/test_runtime/check.h"

int main(void)
{
    const UmiStudioRuntimeLayoutPresetDefinition *preset =
        umi_studio_layout_preset_debugging();

    UMI_TEST_REQUIRE(preset != NULL);
    UMI_TEST_REQUIRE(strcmp(preset->preset_id,
                            "umicom.studio.layout.debugging") == 0);
    /* A debugging layout must keep its essential tools visible, while future
     * diagnostic panels may be added without invalidating this contract. */
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_DEBUG));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_EDITOR));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_PROBLEMS));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_OUTPUT));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_validate(preset) ==
                     UMI_STATUS_OK);
    return 0;
}
