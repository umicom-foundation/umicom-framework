/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_profile_apply.c
 *
 * PURPOSE:
 *   Verify a profile configures canonical groups, bindings and endpoints.
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
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(fixture.host->link_service->groups.count == 2U);
    assert(fixture.host->endpoints.count == 3U);
    assert(strcmp(
        umi_workbench_context_host_active_group(fixture.host),
        "blue") == 0);
    assert(fixture.host->link_service->bindings.count == 3U);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
