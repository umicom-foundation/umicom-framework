/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/navigate_forward.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias navigate.forward.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/navigate_forward.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_navigate_forward(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "navigate.forward",
        .title = "Forward",
        .category = "Navigate",
        .description = "Navigate forward.",
        .target_command_id = "ide.navigate.forward",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 1
    };
    return &definition;
}
