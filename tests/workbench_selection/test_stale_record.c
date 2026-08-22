/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_stale_record.c
 *
 * PURPOSE:
 *   Verify the stale-selection record contract and hash stability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/stale_record.h"

int main(void)
{
    UmiWorkbenchSelectionStaleRecord record;
    uint64_t hash;

    umi_workbench_selection_stale_record_init(
        &record, "stale_record-record");
    assert(umi_workbench_selection_stale_record_validate(
        &record) == UMI_STATUS_OK);
    assert(umi_workbench_selection_stale_record_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_stale_record_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_stale_record_set_related(
        &record, "related") == UMI_STATUS_OK);
    assert(umi_workbench_selection_stale_record_set_group(
        &record, "green") == UMI_STATUS_OK);
    assert(umi_workbench_selection_stale_record_set_description(
        &record, "description") == UMI_STATUS_OK);
    record.selection_kind = UMI_WORKBENCH_SELECTION_TEST;
    record.context_kind = UMI_CONTEXT_KIND_SELECTION;
    record.state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;
    record.count = 3U;

    hash = umi_workbench_selection_stale_record_hash(&record);
    assert(hash != 0U);
    umi_workbench_selection_stale_record_touch(
        &record, 3U, 500U);
    assert(record.sequence == 3U);
    assert(record.timestamp_ms == 500U);
    assert(strcmp(record.related_id, "related") == 0);
    return 0;
}
