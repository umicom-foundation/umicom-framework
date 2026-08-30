/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_drag_drop_link.c
 *
 * PURPOSE:
 *   Verify the drag-and-drop context-link operation contract, mutation, validation and stable hashing.
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

#include "umicom/workbench_context_link/drag_drop_link.h"

int main(void)
{
    UmiWorkbenchContextLinkDragDropLink record;
    UmiWorkbenchContextLinkDragDropLink copy;
    uint64_t first_hash;
    umi_workbench_context_link_drag_drop_link_init(&record, "drag_drop_link-id");
    assert(umi_workbench_context_link_drag_drop_link_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_drag_drop_link_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_drag_drop_link_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_drag_drop_link_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_drag_drop_link_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_drag_drop_link_hash(&copy) == first_hash);
    umi_workbench_context_link_drag_drop_link_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.operation_id, record.operation_id) == 0);
    return 0;
}
