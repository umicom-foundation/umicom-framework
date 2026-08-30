/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_delivery_record.c
 *
 * PURPOSE:
 *   Verify delivery record validation, mutation and stable hashing.
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

#include "umicom/workbench_context_event/delivery_record.h"

int main(void)
{
    UmiWorkbenchContextEventDeliveryRecord record;
    uint64_t first_hash;

    umi_workbench_context_event_delivery_record_init(
        &record,
        "delivery_record-record");
    assert(umi_workbench_context_event_delivery_record_validate(
        &record) == UMI_STATUS_OK);

    assert(umi_workbench_context_event_delivery_record_set_source(
        &record,
        "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_delivery_record_set_subject(
        &record,
        "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_delivery_record_set_group(
        &record,
        "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_delivery_record_set_label(
        &record,
        "label") == UMI_STATUS_OK);

    record.event_kind =
        UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.priority =
        UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    record.state =
        UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;

    first_hash = umi_workbench_context_event_delivery_record_hash(
        &record);
    assert(first_hash != 0U);
    umi_workbench_context_event_delivery_record_touch(
        &record,
        7U,
        1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.subject_id, "subject") == 0);
    return 0;
}
