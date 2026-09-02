/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/visual_design.c
 * PURPOSE: Arrange the canvas and synchronized authoring panels for UI work.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/visual_design.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_VISUAL_DESIGNER,
    UMI_STUDIO_SURFACE_COMPONENT_PALETTE,
    UMI_STUDIO_SURFACE_COMPONENT_STRUCTURE,
    UMI_STUDIO_SURFACE_OBJECT_INSPECTOR,
    UMI_STUDIO_SURFACE_DESIGN_CODE,
    UMI_STUDIO_SURFACE_DESIGN_MIXED,
    UMI_STUDIO_SURFACE_DESIGN_PREVIEW,
    UMI_STUDIO_SURFACE_PROBLEMS,
    UMI_STUDIO_SURFACE_OUTPUT
};

/* Return the immutable set of surfaces used for visual application design. */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_visual_design(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.visual-design",
        .title = "Visual Application Design",
        .description =
            "Design, code, mixed and preview surfaces with synchronized tools.",
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
