/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_source_control.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.source-control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_source_control.h"

/*
 * Provide the studio command alias view source control operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_source_control(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.source-control",
        .title = "Source Control",
        .category = "View",
        .description = "Open Source Control.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_SOURCE_CONTROL,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
