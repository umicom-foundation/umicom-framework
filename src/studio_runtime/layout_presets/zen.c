/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/zen.c
 *
 * PURPOSE:
 *   Define the Zen Coding Studio layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/zen.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_EDITOR
};

/*
 * Provide the studio layout preset zen operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_zen(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.zen",
        .title = "Zen Coding",
        .description = "Distraction-free editor-only layout.",
        .perspective_id = "umicom.perspective.integrated-development",
        .visible_surfaces = VISIBLE,
        .visible_surface_count = sizeof(VISIBLE) / sizeof(VISIBLE[0]),
        .primary_sidebar_visible = 0,
        .secondary_sidebar_visible = 0,
        .bottom_panel_visible = 0,
        .status_bar_visible = 0,
        .zen_mode = 1
    };
    return &preset;
}
