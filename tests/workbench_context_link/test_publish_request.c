/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_publish_request.c
 *
 * PURPOSE:
 *   Verify the context publication request contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/publish_request.h"

int main(void)
{
    UmiWorkbenchContextLinkPublishRequest record;
    UmiWorkbenchContextLinkPublishRequest copy;
    uint64_t first_hash;
    umi_workbench_context_link_publish_request_init(&record, "publish_request-id");
    assert(umi_workbench_context_link_publish_request_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_publish_request_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_publish_request_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_publish_request_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_publish_request_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_publish_request_hash(&copy) == first_hash);
    umi_workbench_context_link_publish_request_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.request_id, record.request_id) == 0);
    return 0;
}
