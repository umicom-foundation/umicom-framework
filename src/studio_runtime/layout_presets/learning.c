/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/learning.c
 *
 * PURPOSE:
 *   Define a Studio layout for learning C while editing, building and reading
 *   documentation in the same customizable workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/learning.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_EXPLORER,
    UMI_STUDIO_SURFACE_EDITOR,
    UMI_STUDIO_SURFACE_LEARNING,
    UMI_STUDIO_SURFACE_DOCUMENTATION,
    UMI_STUDIO_SURFACE_BUILD,
    UMI_STUDIO_SURFACE_PROBLEMS,
    UMI_STUDIO_SURFACE_TERMINAL
};

/* Return the shared arrangement used when lessons accompany real C work. */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_learning(void)
{
    /* A semantic preset lists useful surfaces; the frontend remains free to
     * arrange them for the current monitor size and saved user preferences. */
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.learning",
        .title = "C Development and Learning",
        .description =
            "Editor, lessons, documentation, build, problems and terminal in one workspace.",
        .perspective_id = "umicom.perspective.integrated-development",
        .visible_surfaces = VISIBLE,
        .visible_surface_count = sizeof(VISIBLE) / sizeof(VISIBLE[0]),
        .primary_sidebar_visible = 1,
        .secondary_sidebar_visible = 1,
        .bottom_panel_visible = 1,
        .status_bar_visible = 1,
        .zen_mode = 0
    };
    return &preset;
}
