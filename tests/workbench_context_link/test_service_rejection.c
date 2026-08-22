/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_service_rejection.c
 *
 * PURPOSE:
 *   Verify group kind policy rejects incompatible publications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/payload_builder.h"
#include "umicom/workbench_context_link/service.h"

int main(void)
{
    UmiWorkbenchContextLinkService service;
    UmiContextPayload payload;
    UmiWorkbenchContextLinkDeliveryBatch deliveries;
    const uint64_t mask = umi_workbench_context_link_kind_mask(UMI_CONTEXT_KIND_PROJECT);
    umi_workbench_context_link_service_init(&service);
    assert(umi_workbench_context_link_service_define_group(
        &service, "blue", "Blue", UMI_CONTEXT_COLOUR_BLUE, mask,
        UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_build_instrument(
        &payload, "instrument-context", "trader", "watchlist",
        "instrument-1", "ES", "CME") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_service_publish(
        &service, "blue", NULL, &payload, 10U, &deliveries) == UMI_STATUS_PERMISSION_DENIED);
    assert(service.metrics.rejected_count == 1U);
    umi_workbench_context_link_service_destroy(&service);
    return 0;
}
