/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_output.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.output.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_output.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_output(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.output",
        .title = "Output",
        .category = "View",
        .description = "Open Output.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_OUTPUT,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
