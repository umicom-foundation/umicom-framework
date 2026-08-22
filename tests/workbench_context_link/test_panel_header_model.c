/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_panel_header_model.c
 *
 * PURPOSE:
 *   Verify the linked-panel header view model contract, mutation, validation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/panel_header_model.h"

int main(void)
{
    UmiWorkbenchContextLinkPanelHeaderModel record;
    UmiWorkbenchContextLinkPanelHeaderModel copy;
    uint64_t first_hash;
    umi_workbench_context_link_panel_header_model_init(&record, "panel_header_model-id");
    assert(umi_workbench_context_link_panel_header_model_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_panel_header_model_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_panel_header_model_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_panel_header_model_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_panel_header_model_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_panel_header_model_hash(&copy) == first_hash);
    umi_workbench_context_link_panel_header_model_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.model_id, record.model_id) == 0);
    return 0;
}
