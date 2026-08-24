/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_terminal.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.terminal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_terminal.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_terminal(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.terminal",
        .title = "Terminal",
        .category = "View",
        .description = "Open Terminal.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_TERMINAL,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
