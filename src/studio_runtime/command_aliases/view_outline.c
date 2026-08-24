/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_outline.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.outline.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_outline.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_outline(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.outline",
        .title = "Outline",
        .category = "View",
        .description = "Open Outline.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_OUTLINE,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
