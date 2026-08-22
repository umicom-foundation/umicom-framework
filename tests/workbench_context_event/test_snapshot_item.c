/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_snapshot_item.c
 *
 * PURPOSE:
 *   Verify snapshot item validation, mutation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_event/snapshot_item.h"

int main(void)
{
    UmiWorkbenchContextEventSnapshotItem record;
    uint64_t first_hash;

    umi_workbench_context_event_snapshot_item_init(
        &record,
        "snapshot_item-record");
    assert(umi_workbench_context_event_snapshot_item_validate(
        &record) == UMI_STATUS_OK);

    assert(umi_workbench_context_event_snapshot_item_set_source(
        &record,
        "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_snapshot_item_set_subject(
        &record,
        "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_snapshot_item_set_group(
        &record,
        "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_snapshot_item_set_label(
        &record,
        "label") == UMI_STATUS_OK);

    record.event_kind =
        UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.priority =
        UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    record.state =
        UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;

    first_hash = umi_workbench_context_event_snapshot_item_hash(
        &record);
    assert(first_hash != 0U);
    umi_workbench_context_event_snapshot_item_touch(
        &record,
        7U,
        1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.subject_id, "subject") == 0);
    return 0;
}
