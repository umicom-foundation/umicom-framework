/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_command_catalogue.c
 *
 * PURPOSE:
 *   Verify the complete developer command catalogue is valid and globally
 *   unique, protecting the shared menu/toolbar/AI command namespace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_workbench/command_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(umi_developer_workbench_command_catalogue_validate() ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_command_catalogue_count() >= 100U);

    assert(umi_developer_workbench_command_catalogue_find(
        "build.build") != NULL);
    assert(umi_developer_workbench_command_catalogue_find(
        "test.run-all") != NULL);
    assert(umi_developer_workbench_command_catalogue_find(
        "ai.agent.open") != NULL);
    assert(umi_developer_workbench_command_catalogue_find(
        "workbench.command-palette") != NULL);

    return 0;
}
