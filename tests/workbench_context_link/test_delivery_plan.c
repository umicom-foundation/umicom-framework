/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_delivery_plan.c
 *
 * PURPOSE:
 *   Verify the context delivery plan contract, mutation, validation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/delivery_plan.h"

int main(void)
{
    UmiWorkbenchContextLinkDeliveryPlan record;
    UmiWorkbenchContextLinkDeliveryPlan copy;
    uint64_t first_hash;
    umi_workbench_context_link_delivery_plan_init(&record, "delivery_plan-id");
    assert(umi_workbench_context_link_delivery_plan_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_plan_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_plan_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_delivery_plan_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_delivery_plan_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_delivery_plan_hash(&copy) == first_hash);
    umi_workbench_context_link_delivery_plan_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.plan_id, record.plan_id) == 0);
    return 0;
}
