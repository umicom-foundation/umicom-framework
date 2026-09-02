/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_history_projection.c
 *
 * PURPOSE:
 *   Verify reverse chronological context history.
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
#include "umicom/workbench_context_host/history_projection.h"

#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostHistoryProjection projection;
    UmiContextPayload first;
    UmiContextPayload second;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&first, "first", "test.source") == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(&second, "second", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &first, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &second, 20U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_history_projection_build(
        fixture.host, "blue", &projection) == UMI_STATUS_OK);
    assert(projection.count == 2U);
    assert(strcmp(projection.rows[0].context_id, "second") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
