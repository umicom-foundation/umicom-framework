/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_service_pin.c
 *
 * PURPOSE:
 *   Verify current contexts can be pinned with stable identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_link/service.h"

int main(void)
{
    UmiWorkbenchContextLinkService service;
    UmiContextPayload payload;
    UmiWorkbenchContextLinkDeliveryBatch deliveries;
    umi_workbench_context_link_service_init(&service);
    assert(umi_workbench_context_link_service_define_group(
        &service, "blue", "Blue", UMI_CONTEXT_COLOUR_BLUE,
        UMI_WORKBENCH_CONTEXT_LINK_ALL_KINDS_MASK,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_build_selection(
        &payload, "selection-context", "studio", "explorer",
        "selection-1", "file", "main.c") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_service_publish(
        &service, "blue", NULL, &payload, 1U, &deliveries) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_service_pin_current(
        &service, "pin-1", "blue", 2U) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_pin_store_find(&service.pins, "pin-1") != NULL);
    umi_workbench_context_link_service_destroy(&service);
    return 0;
}
