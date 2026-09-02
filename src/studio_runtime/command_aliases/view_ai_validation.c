/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_ai_validation.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.ai-validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_ai_validation.h"

/*
 * Provide the studio command alias view ai validation operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_ai_validation(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.ai-validation",
        .title = "AI Validation",
        .category = "View",
        .description = "Open AI validation.",
        .target_command_id = "ai.developer.open-validation",
        .surface = UMI_STUDIO_SURFACE_AI_VALIDATION,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
