/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_search.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.search.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_search.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_search(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.search",
        .title = "Search",
        .category = "View",
        .description = "Open Search.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_SEARCH,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
