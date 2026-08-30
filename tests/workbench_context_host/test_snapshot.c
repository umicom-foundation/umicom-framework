/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_snapshot.c
 *
 * PURPOSE:
 *   Verify host snapshot counts and routing revisions.
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

int main(void)
{

    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostSnapshot snapshot;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_snapshot(
        fixture.host, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.endpoint_count == 3U);
    assert(snapshot.group_count == 2U);
    assert(strcmp(snapshot.active_group_id, "blue") == 0);
    assert(snapshot.revision == fixture.host->revision);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
