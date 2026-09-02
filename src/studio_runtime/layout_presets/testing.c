/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/testing.c
 *
 * PURPOSE:
 *   Define the Testing Studio layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/testing.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_TEST_EXPLORER, UMI_STUDIO_SURFACE_EDITOR, UMI_STUDIO_SURFACE_PROBLEMS, UMI_STUDIO_SURFACE_OUTPUT
};

/*
 * Provide the studio layout preset testing operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_testing(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.testing",
        .title = "Testing",
        .description = "Test Explorer, Editor, Problems and Output for test-driven work.",
        .perspective_id = "umicom.perspective.integrated-development",
        .visible_surfaces = VISIBLE,
        .visible_surface_count = sizeof(VISIBLE) / sizeof(VISIBLE[0]),
        .primary_sidebar_visible = 1,
        .secondary_sidebar_visible = 0,
        .bottom_panel_visible = 1,
        .status_bar_visible = 1,
        .zen_mode = 0
    };
    return &preset;
}
