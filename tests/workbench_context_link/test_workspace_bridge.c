/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_workspace_bridge.c
 *
 * PURPOSE:
 *   Verify the workspace context bridge contract, mutation, validation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/workspace_bridge.h"

int main(void)
{
    UmiWorkbenchContextLinkWorkspaceBridge record;
    UmiWorkbenchContextLinkWorkspaceBridge copy;
    uint64_t first_hash;
    umi_workbench_context_link_workspace_bridge_init(&record, "workspace_bridge-id");
    assert(umi_workbench_context_link_workspace_bridge_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_workspace_bridge_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_workspace_bridge_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_workspace_bridge_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_workspace_bridge_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_workspace_bridge_hash(&copy) == first_hash);
    umi_workbench_context_link_workspace_bridge_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.bridge_id, record.bridge_id) == 0);
    return 0;
}
