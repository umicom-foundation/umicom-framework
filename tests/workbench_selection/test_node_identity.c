/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_node_identity.c
 *
 * PURPOSE:
 *   Verify the structured node identity contract and hash stability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/node_identity.h"

int main(void)
{
    UmiWorkbenchSelectionNodeIdentity record;
    uint64_t hash;

    umi_workbench_selection_node_identity_init(
        &record, "node_identity-record");
    assert(umi_workbench_selection_node_identity_validate(
        &record) == UMI_STATUS_OK);
    assert(umi_workbench_selection_node_identity_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_node_identity_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_node_identity_set_related(
        &record, "related") == UMI_STATUS_OK);
    assert(umi_workbench_selection_node_identity_set_group(
        &record, "green") == UMI_STATUS_OK);
    assert(umi_workbench_selection_node_identity_set_description(
        &record, "description") == UMI_STATUS_OK);
    record.selection_kind = UMI_WORKBENCH_SELECTION_TEST;
    record.context_kind = UMI_CONTEXT_KIND_SELECTION;
    record.state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    record.count = 3U;

    hash = umi_workbench_selection_node_identity_hash(&record);
    assert(hash != 0U);
    umi_workbench_selection_node_identity_touch(
        &record, 3U, 500U);
    assert(record.sequence == 3U);
    assert(record.timestamp_ms == 500U);
    assert(strcmp(record.related_id, "related") == 0);
    return 0;
}
