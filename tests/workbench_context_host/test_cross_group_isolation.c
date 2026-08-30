/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_cross_group_isolation.c
 *
 * PURPOSE:
 *   Verify a blue-group publication cannot leak into a red-group endpoint.
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
#include "umicom/workbench_context_host/host.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiContextPayload payload;
    const UmiWorkbenchContextHostInbox *red_inbox;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_project_payload(
        &payload, "project", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 100U) == UMI_STATUS_OK);
    red_inbox = umi_workbench_context_host_inbox_registry_find_const(
        &fixture.host->inboxes, "test.trading.endpoint");
    assert(red_inbox == NULL || red_inbox->count == 0U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
