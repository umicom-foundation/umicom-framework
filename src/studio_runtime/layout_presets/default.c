/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/default.c
 *
 * PURPOSE:
 *   Define the Default Development Studio layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/default.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_EXPLORER, UMI_STUDIO_SURFACE_EDITOR, UMI_STUDIO_SURFACE_PROBLEMS, UMI_STUDIO_SURFACE_OUTPUT
};

const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_default(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.default",
        .title = "Default Development",
        .description = "Balanced Explorer, Editor, Problems and Output development layout.",
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
