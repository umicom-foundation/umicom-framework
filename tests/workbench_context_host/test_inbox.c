/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_inbox.c
 *
 * PURPOSE:
 *   Verify FIFO delivery queue behavior.
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

    UmiWorkbenchContextHostInbox inbox;
    UmiWorkbenchContextHostDelivery delivery;
    UmiWorkbenchContextHostDelivery popped;
    UmiContextPayload payload;
    umi_workbench_context_host_inbox_init(&inbox, "target", "test.target");
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
    assert(umi_workbench_context_host_inbox_push(&inbox, &delivery) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_inbox_peek(&inbox) != NULL);
    assert(umi_workbench_context_host_inbox_pop(
        &inbox, &popped) == UMI_STATUS_OK);
    assert(strcmp(popped.delivery_id, "delivery") == 0);
    assert(inbox.count == 0U);
    umi_workbench_context_host_inbox_destroy(&inbox);
    return 0;
}
