/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_problems.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.problems.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_problems.h"

const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_problems(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.problems",
        .title = "Problems",
        .category = "View",
        .description = "Open Problems.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_PROBLEMS,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
