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
#include <string.h>
#include "umicom/studio_runtime/layout_presets/ai_development.h"
#include "umicom/test_runtime/check.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiStudioRuntimeLayoutPresetDefinition *preset =
        umi_studio_layout_preset_ai_development();

    UMI_TEST_REQUIRE(preset != NULL);
    UMI_TEST_REQUIRE(strcmp(
        preset->preset_id, "umicom.studio.layout.ai-development") == 0);
    /* Test the semantic contents rather than a stale literal count because a
     * future optional tool may be added without removing these core surfaces. */
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_EXPLORER));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_EDITOR));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_AI_CHAT));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_PROBLEMS));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_contains_surface(
        preset, UMI_STUDIO_SURFACE_AI_TOOLS));
    UMI_TEST_REQUIRE(umi_studio_layout_preset_validate(preset) ==
                     UMI_STATUS_OK);
    return 0;
}
