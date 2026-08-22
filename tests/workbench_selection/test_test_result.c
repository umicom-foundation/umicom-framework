/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_test_result.c
 *
 * PURPOSE:
 *   Verify the structured test result contract, mutation and hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/test_result.h"

int main(void)
{
    UmiWorkbenchSelectionTestResult record;
    uint64_t hash;

    umi_workbench_selection_test_result_init(
        &record,
        "test_result-record");
    assert(umi_workbench_selection_test_result_validate(
        &record) == UMI_STATUS_OK);
    assert(umi_workbench_selection_test_result_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_test_result_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_test_result_set_secondary(
        &record, "secondary") == UMI_STATUS_OK);
    assert(umi_workbench_selection_test_result_set_group(
        &record, "blue") == UMI_STATUS_OK);
    assert(umi_workbench_selection_test_result_set_label(
        &record, "label") == UMI_STATUS_OK);

    record.selection_kind = UMI_WORKBENCH_SELECTION_PROJECT;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.activation = UMI_WORKBENCH_SELECTION_ACTIVATION_OPEN;
    record.state = UMI_WORKBENCH_SELECTION_STATE_RESOLVED;

    hash = umi_workbench_selection_test_result_hash(&record);
    assert(hash != 0U);
    umi_workbench_selection_test_result_touch(
        &record, 7U, 1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    return 0;
}
