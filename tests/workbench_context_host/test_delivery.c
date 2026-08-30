/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_delivery.c
 *
 * PURPOSE:
 *   Verify typed delivery validation requires route and payload identity.
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

    UmiWorkbenchContextHostDelivery delivery;
    UmiContextPayload payload;
    umi_workbench_context_host_delivery_init(&delivery, "delivery");
    assert(umi_wch_test_selection_payload(
        &payload, "selection", "test.source") == UMI_STATUS_OK);
    delivery.payload = payload;
    assert(umi_workbench_context_host_copy_text(
        delivery.endpoint_id, sizeof(delivery.endpoint_id), "target") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        delivery.panel_id, sizeof(delivery.panel_id), "test.target") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        delivery.application_id, sizeof(delivery.application_id), "org.umicom.test") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_copy_text(
        delivery.group_id, sizeof(delivery.group_id), "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_delivery_validate(
        &delivery) == UMI_STATUS_OK);
    return 0;
}
