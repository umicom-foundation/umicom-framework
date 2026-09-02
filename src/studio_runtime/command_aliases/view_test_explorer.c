/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/command_aliases/view_test_explorer.c
 *
 * PURPOSE:
 *   Define Studio compatibility alias view.test-explorer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/command_aliases/view_test_explorer.h"

/*
 * Provide the studio command alias view test explorer operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeCommandAliasDefinition *
umi_studio_command_alias_view_test_explorer(void)
{
    static const UmiStudioRuntimeCommandAliasDefinition definition = {
        .alias_id = "view.test-explorer",
        .title = "Test Explorer",
        .category = "View",
        .description = "Open Test Explorer.",
        .target_command_id = "",
        .surface = UMI_STUDIO_SURFACE_TEST_EXPLORER,
        .activate_surface = 1,
        .mutates_state = 0
    };
    return &definition;
}
