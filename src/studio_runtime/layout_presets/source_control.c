/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/source_control.c
 *
 * PURPOSE:
 *   Define the Source Control Studio layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/source_control.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_SOURCE_CONTROL, UMI_STUDIO_SURFACE_EDITOR, UMI_STUDIO_SURFACE_PROBLEMS, UMI_STUDIO_SURFACE_OUTPUT
};

const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_source_control(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.source-control",
        .title = "Source Control",
        .description = "Source Control, Editor, Problems and Output for review and commit workflows.",
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
