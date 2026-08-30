/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_pin_across_group_switch.c
 *
 * PURPOSE:
 *   Verify pinned contexts survive active-group changes.
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
    const UmiWorkbenchContextLinkPinItem *pin;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(
        &payload, "selection", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &payload, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_pin(
        fixture.host, "pin-1", 20U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_set_active_group(
        fixture.host, "red") == UMI_STATUS_OK);
    pin = umi_workbench_context_link_pin_store_find(
        &fixture.host->link_service->pins, "pin-1");
    assert(pin != NULL);
    assert(strcmp(pin->group_id, "blue") == 0);
    assert(strcmp(pin->payload.identity.context_id, "selection") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
