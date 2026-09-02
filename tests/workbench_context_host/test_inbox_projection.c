/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_inbox_projection.c
 *
 * PURPOSE:
 *   Verify queue and drop projections are aggregated by endpoint.
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
#include "umicom/workbench_context_host/inbox_projection.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostInboxProjection projection;
    UmiContextPayload payload;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_inbox_projection_build(
        fixture.host, &projection) == UMI_STATUS_OK);
    assert(projection.count == 1U);
    assert(projection.total_queued == 1U);
    assert(strcmp(projection.rows[0].next_context_id, "selection") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
