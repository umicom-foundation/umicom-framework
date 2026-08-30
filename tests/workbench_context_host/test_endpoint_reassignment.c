/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_endpoint_reassignment.c
 *
 * PURPOSE:
 *   Verify a panel reassignment immediately changes its routed colour group.
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
    UmiWorkbenchContextHostDelivery delivery;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_assign_endpoint_group(
        fixture.host,
        "test.target.endpoint",
        "red",
        UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(
        &payload, "blue-selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_pop_delivery(
        fixture.host,
        "test.target.endpoint",
        &delivery) == UMI_STATUS_NOT_FOUND);
    assert(umi_workbench_context_host_publish(
        fixture.host, "red", "test.trading", &payload, 20U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_pop_delivery(
        fixture.host,
        "test.target.endpoint",
        &delivery) == UMI_STATUS_OK);
    assert(strcmp(delivery.group_id, "red") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
