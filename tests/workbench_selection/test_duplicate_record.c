/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_duplicate_record.c
 *
 * PURPOSE:
 *   Verify the duplicate selection record contract, mutation and hashing.
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

#include "umicom/workbench_selection/duplicate_record.h"

int main(void)
{
    UmiWorkbenchSelectionDuplicateRecord record;
    uint64_t hash;

    umi_workbench_selection_duplicate_record_init(
        &record,
        "duplicate_record-record");
    assert(umi_workbench_selection_duplicate_record_validate(
        &record) == UMI_STATUS_OK);
    assert(umi_workbench_selection_duplicate_record_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_duplicate_record_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_duplicate_record_set_secondary(
        &record, "secondary") == UMI_STATUS_OK);
    assert(umi_workbench_selection_duplicate_record_set_group(
        &record, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_selection_duplicate_record_set_label(
        &record, "label") == UMI_STATUS_OK);

    record.selection_kind = UMI_WORKBENCH_SELECTION_PROJECT;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.activation = UMI_WORKBENCH_SELECTION_ACTIVATION_OPEN;
    record.state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;

    hash = umi_workbench_selection_duplicate_record_hash(&record);
    assert(hash != 0U);
    umi_workbench_selection_duplicate_record_touch(
        &record, 7U, 1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    return 0;
}
