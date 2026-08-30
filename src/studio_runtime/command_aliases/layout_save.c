/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/layout_save.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias layout.save.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/layout_save.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_layout_save(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "layout.save",
        .title = "Save Layout",
        .category = "Window",
        .description = "Save the semantic Studio layout.",
        .target_command_id = "studio.layout.save",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 1
    };
    return &definition;
}
