/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_ai_review.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.ai-review.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_ai_review.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_ai_review(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.ai-review",
        .title = "AI Patch Review",
        .category = "View",
        .description = "Open AI Patch Review.",
        .target_command_id = "ide.ai.open-review",
        .surface = UMI_STUDIO_SURFACE_AI_REVIEW,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
