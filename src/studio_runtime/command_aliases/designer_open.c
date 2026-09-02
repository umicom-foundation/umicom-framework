/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/designer_open.c
 * PURPOSE: Connect the Tools menu designer command to the visual-design layout.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/designer_open.h"

/* Map the public Tools command to the complete named design layout. */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_designer_open(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "designer.open",
        .title = "Visual Designer",
        .category = "Design",
        .description = "Open the visual application design workspace.",
        .target_command_id = "studio.layout.visual-design",
        .surface = UMI_STUDIO_SURFACE_VISUAL_DESIGNER,
        .activate_surface = 1,
        .mutates_state = 1
    };
    return &definition;
}
