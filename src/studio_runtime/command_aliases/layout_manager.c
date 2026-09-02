/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/layout_manager.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias layout.manager.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/layout_manager.h"

/*
 * Provide the studio command alias layout manager operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_layout_manager(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "layout.manager",
        .title = "Layout Manager",
        .category = "Window",
        .description = "Open the semantic Studio layout manager.",
        .target_command_id = "studio.layout.manager",
        .surface = UMI_STUDIO_SURFACE_EDITOR,
        .activate_surface = 0,
        .mutates_state = 0
    };
    return &definition;
}
