/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_activation_policy.c
 *
 * PURPOSE:
 *   Verify the panel activation policy contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/activation_policy.h"

int main(void)
{
    UmiWorkbenchContextLinkActivationPolicy record;
    UmiWorkbenchContextLinkActivationPolicy copy;
    uint64_t first_hash;
    umi_workbench_context_link_activation_policy_init(&record, "activation_policy-id");
    assert(umi_workbench_context_link_activation_policy_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_activation_policy_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_activation_policy_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_activation_policy_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_activation_policy_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_activation_policy_hash(&copy) == first_hash);
    umi_workbench_context_link_activation_policy_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.policy_id, record.policy_id) == 0);
    return 0;
}
