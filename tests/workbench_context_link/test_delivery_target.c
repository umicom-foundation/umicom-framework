/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_delivery_target.c
 *
 * PURPOSE:
 *   Verify the context delivery target contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/delivery_target.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextLinkDeliveryTarget record;
    UmiWorkbenchContextLinkDeliveryTarget copy;
    uint64_t first_hash;
    umi_workbench_context_link_delivery_target_init(&record, "delivery_target-id");
    assert(umi_workbench_context_link_delivery_target_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_target_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_target_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_delivery_target_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_delivery_target_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_target_hash(&copy) == first_hash);
    umi_workbench_context_link_delivery_target_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.target_id, record.target_id) == 0);
    return 0;
}
