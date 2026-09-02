/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_health.c
 *
 * PURPOSE:
 *   Verify healthy and suspended host health states.
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
    UmiWorkbenchContextHostHealth health;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_health_evaluate(
        fixture.host, &health) == UMI_STATUS_OK);
    assert(health.state == UMI_WORKBENCH_CONTEXT_HOST_HEALTH_HEALTHY);
    umi_workbench_context_host_set_suspended(fixture.host, true);
    assert(umi_workbench_context_host_health_evaluate(
        fixture.host, &health) == UMI_STATUS_OK);
    assert(health.state == UMI_WORKBENCH_CONTEXT_HOST_HEALTH_DEGRADED);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
