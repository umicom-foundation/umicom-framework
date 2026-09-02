/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_command.c
 *
 * PURPOSE:
 *   Verify active-group, suspend and resume commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostCommand command;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    umi_workbench_context_host_command_init(
        &command, UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SET_ACTIVE_GROUP);
    assert(umi_workbench_context_host_copy_text(
        command.group_id, sizeof(command.group_id), "red") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_command_execute(
        fixture.host, &command) == UMI_STATUS_OK);
    assert(strcmp(fixture.host->active_group_id, "red") == 0);
    umi_workbench_context_host_command_init(
        &command, UMI_WORKBENCH_CONTEXT_HOST_COMMAND_SUSPEND);
    assert(umi_workbench_context_host_command_execute(
        fixture.host, &command) == UMI_STATUS_OK);
    assert(fixture.host->suspended);
    umi_workbench_context_host_command_init(
        &command, UMI_WORKBENCH_CONTEXT_HOST_COMMAND_RESUME);
    assert(umi_workbench_context_host_command_execute(
        fixture.host, &command) == UMI_STATUS_OK);
    assert(!fixture.host->suspended);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
