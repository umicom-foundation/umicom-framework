/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_context_diff.c
 *
 * PURPOSE:
 *   Verify the context comparison record contract, mutation, validation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_link/context_diff.h"

int main(void)
{
    UmiWorkbenchContextLinkContextDiff record;
    UmiWorkbenchContextLinkContextDiff copy;
    uint64_t first_hash;
    umi_workbench_context_link_context_diff_init(&record, "context_diff-id");
    assert(umi_workbench_context_link_context_diff_validate(&record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_context_diff_set_primary(&record, "primary") == UMI_STATUS_OK);
    assert(umi_workbench_context_link_context_diff_set_secondary(&record, "secondary") == UMI_STATUS_OK);
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.colour = UMI_CONTEXT_COLOUR_BLUE;
    record.mode = UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
    record.state = UMI_WORKBENCH_CONTEXT_LINK_STATE_ACTIVE;
    first_hash = umi_workbench_context_link_context_diff_hash(&record);
    assert(first_hash != 0U);
    assert(umi_workbench_context_link_context_diff_copy(&copy, &record) == UMI_STATUS_OK);
    assert(umi_workbench_context_link_context_diff_hash(&copy) == first_hash);
    umi_workbench_context_link_context_diff_touch(&copy, 9U, 1000U);
    assert(copy.sequence == 9U);
    assert(copy.timestamp_ms == 1000U);
    assert(copy.revision > record.revision);
    assert(strcmp(copy.diff_id, record.diff_id) == 0);
    return 0;
}
