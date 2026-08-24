/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_ai_approvals.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.ai-approvals.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_ai_approvals.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_ai_approvals(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.ai-approvals",
        .title = "AI Approvals",
        .category = "View",
        .description = "Open AI approvals.",
        .target_command_id = "ai.developer.open-approvals",
        .surface = UMI_STUDIO_SURFACE_AI_APPROVALS,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
