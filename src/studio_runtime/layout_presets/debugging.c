/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/debugging.c
 *
 * PURPOSE:
 *   Define the Debugging Studio layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/debugging.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_DEBUG, UMI_STUDIO_SURFACE_EDITOR, UMI_STUDIO_SURFACE_PROBLEMS, UMI_STUDIO_SURFACE_OUTPUT, UMI_STUDIO_SURFACE_TERMINAL
};

/*
 * Provide the studio layout preset debugging operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_debugging(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.debugging",
        .title = "Debugging",
        .description = "Debug-focused layout with Run and Debug, Editor, Problems and Output.",
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
