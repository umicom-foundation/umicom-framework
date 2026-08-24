/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/review.c
 *
 * PURPOSE:
 *   Define the AI Patch Review Studio layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/review.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_AI_REVIEW, UMI_STUDIO_SURFACE_AI_APPROVALS, UMI_STUDIO_SURFACE_AI_VALIDATION, UMI_STUDIO_SURFACE_PROBLEMS
};

const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_review(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.review",
        .title = "AI Patch Review",
        .description = "AI Patch Review with approvals and validation evidence.",
        .perspective_id = "umicom.perspective.integrated-development",
        .visible_surfaces = VISIBLE,
        .visible_surface_count = sizeof(VISIBLE) / sizeof(VISIBLE[0]),
        .primary_sidebar_visible = 0,
        .secondary_sidebar_visible = 1,
        .bottom_panel_visible = 1,
        .status_bar_visible = 1,
        .zen_mode = 0
    };
    return &preset;
}
