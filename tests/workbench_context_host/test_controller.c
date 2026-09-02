/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_controller.c
 *
 * PURPOSE:
 *   Verify Workbench Context Host Slave Controller lifecycle.
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
    UmiWorkbenchContextHostSlaveController controller;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    umi_workbench_context_host_slave_controller_init(
        &controller, fixture.host);
    assert(umi_workbench_context_host_slave_controller_start(
        &controller) == UMI_STATUS_OK);
    assert(controller.state ==
        UMI_WORKBENCH_CONTEXT_HOST_CONTROLLER_RUNNING);
    assert(umi_workbench_context_host_slave_controller_quiesce(
        &controller) == UMI_STATUS_OK);
    assert(fixture.host->suspended);
    assert(umi_workbench_context_host_slave_controller_resume(
        &controller) == UMI_STATUS_OK);
    assert(!fixture.host->suspended);
    assert(umi_workbench_context_host_slave_controller_stop(
        &controller) == UMI_STATUS_OK);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
