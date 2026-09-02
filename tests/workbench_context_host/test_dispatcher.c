/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_dispatcher.c
 *
 * PURPOSE:
 *   Verify link-service recipients become endpoint inbox deliveries.
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
    UmiContextPayload payload;
    UmiWorkbenchContextHostDelivery delivery;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(
        &payload, "context", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_pop_delivery(
        fixture.host, "test.target.endpoint", &delivery) == UMI_STATUS_OK);
    assert(strcmp(delivery.payload.identity.context_id, "context") == 0);
    assert(delivery.disposition ==
        UMI_WORKBENCH_CONTEXT_HOST_DELIVERY_CONSUMED);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
