/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/layout_presets/ai_development.c
 *
 * PURPOSE:
 *   Define the AI Development Studio layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/layout_presets/ai_development.h"

static const UmiStudioRuntimeSurfaceKind VISIBLE[] = {
    UMI_STUDIO_SURFACE_EXPLORER,
    UMI_STUDIO_SURFACE_EDITOR,
    UMI_STUDIO_SURFACE_AI_CHAT,
    UMI_STUDIO_SURFACE_AI_MODEL_COMPARISON,
    UMI_STUDIO_SURFACE_PROBLEMS,
    UMI_STUDIO_SURFACE_AI_TOOLS
};

/*
 * Provide the studio layout preset ai development operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeLayoutPresetDefinition *
umi_studio_layout_preset_ai_development(void)
{
    static const UmiStudioRuntimeLayoutPresetDefinition preset = {
        .preset_id = "umicom.studio.layout.ai-development",
        .title = "AI Development",
        .description =
            "Editor with AI chat, model comparison, Problems and Tool Activity.",
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
