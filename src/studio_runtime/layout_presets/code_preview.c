/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/code_preview.c
 * PURPOSE: Arrange source, live preview and diagnostics as one editing workflow.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/code_preview.h"

/* These semantic surface identifiers contain no toolkit-specific geometry.
 * Each frontend may therefore present the same workflow on its own platform. */
static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_EXPLORER,
    UMI_STUDIO_SURFACE_EDITOR,
    UMI_STUDIO_SURFACE_LIVE_PREVIEW,
    UMI_STUDIO_SURFACE_PROBLEMS,
    UMI_STUDIO_SURFACE_OUTPUT
};

/* Return the immutable set of surfaces used for code-and-preview work. */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_code_preview(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.code-preview",
        .title = "Code and Live Preview",
        .description =
            "Edit an active document beside its safe, revision-aware preview.",
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
