/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_debug.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.debug.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_debug.h"

/*
 * Provide the studio command alias view debug operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_debug(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.debug",
        .title = "Run and Debug",
        .category = "View",
        .description = "Open Run and Debug.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_DEBUG,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
