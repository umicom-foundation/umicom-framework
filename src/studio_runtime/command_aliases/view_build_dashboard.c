/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_build_dashboard.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.build-dashboard.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_build_dashboard.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_build_dashboard(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.build-dashboard",
        .title = "Build",
        .category = "View",
        .description = "Open Build dashboard.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_BUILD,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
