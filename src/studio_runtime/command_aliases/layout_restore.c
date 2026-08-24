/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/layout_restore.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias layout.restore.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/layout_restore.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_layout_restore(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "layout.restore",
        .title = "Restore Layout",
        .category = "Window",
        .description = "Restore the semantic Studio layout.",
        .target_command_id = "studio.layout.restore",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 1
    };
    return &definition;
}
