/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_ai_tools.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.ai-tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_ai_tools.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_ai_tools(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.ai-tools",
        .title = "AI Tool Activity",
        .category = "View",
        .description = "Open AI tool activity.",
        .target_command_id = "ai.developer.open-tools",
        .surface = UMI_STUDIO_SURFACE_AI_TOOLS,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
