/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_status_model.c
 *
 * PURPOSE:
 *   Verify the context-link status view model contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/status_model.h"

int main(void)
{
    UmiWorkbenchContextLinkStatusModel record;
    UmiWorkbenchContextLinkStatusModel copy;
    uint64_t first_hash;
    umi_workbench_context_link_status_model_init(&record, "status_model-id");
    assert(umi_workbench_context_link_status_model_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_status_model_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_status_model_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_status_model_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_status_model_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_status_model_hash(&copy) == first_hash);
    umi_workbench_context_link_status_model_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.model_id, record.model_id) == 0);
    return 0;
}
