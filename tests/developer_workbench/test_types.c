/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_types.c
 *
 * PURPOSE:
 *   Verify stable workbench command metadata validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/types.h"

int main(void)
{
    UmiDeveloperWorkbenchCommandDefinition definition = {
        "build.build",
        "Build",
        "Build",
        "Compile the active project.",
        UMI_DEVELOPER_WORKBENCH_ACTION_BUILD,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    };

    assert(umi_developer_workbench_command_definition_validate(&definition) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_command_action_text(
        UMI_DEVELOPER_WORKBENCH_ACTION_BUILD) != NULL);
    return 0;
}
