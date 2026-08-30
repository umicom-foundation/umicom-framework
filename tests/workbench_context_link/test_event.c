/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_event.c
 *
 * PURPOSE:
 *   Verify the context-link event record contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/event.h"

int main(void)
{
    UmiWorkbenchContextLinkEvent record;
    UmiWorkbenchContextLinkEvent copy;
    uint64_t first_hash;
    umi_workbench_context_link_event_init(&record, "event-id");
    assert(umi_workbench_context_link_event_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_event_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_event_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_event_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_event_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_event_hash(&copy) == first_hash);
    umi_workbench_context_link_event_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.event_id, record.event_id) == 0);
    return 0;
}
