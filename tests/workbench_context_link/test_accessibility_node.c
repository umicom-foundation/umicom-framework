/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_accessibility_node.c
 *
 * PURPOSE:
 *   Verify the context-link accessibility node contract, mutation, validation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/accessibility_node.h"

int main(void)
{
    UmiWorkbenchContextLinkAccessibilityNode record;
    UmiWorkbenchContextLinkAccessibilityNode copy;
    uint64_t first_hash;
    umi_workbench_context_link_accessibility_node_init(&record, "accessibility_node-id");
    assert(umi_workbench_context_link_accessibility_node_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_accessibility_node_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_accessibility_node_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_accessibility_node_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_accessibility_node_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_accessibility_node_hash(&copy) == first_hash);
    umi_workbench_context_link_accessibility_node_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.node_id, record.node_id) == 0);
    return 0;
}
