/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/navigate_back.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias navigate.back.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/navigate_back.h"

/*
 * Provide the studio command alias navigate back operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_navigate_back(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "navigate.back",
        .title = "Back",
        .category = "Navigate",
        .description = "Navigate back.",
        .target_command_id = "ide.navigate.back",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 1
    };
    return &definition;
}
