/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/ai_assistant_open.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias ai.assistant.open.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/ai_assistant_open.h"

/*
 * Provide the studio command alias ai assistant open operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_ai_assistant_open(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "ai.assistant.open",
        .title = "AI Assistant",
        .category = "AI",
        .description = "Open the AI Developer experience.",
        .target_command_id = "ide.ai.open-overview",
        .surface = UMI_STUDIO_SURFACE_AI_CHAT,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
