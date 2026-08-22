/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_filter.c
 *
 * PURPOSE:
 *   Verify filter validation, mutation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_event/filter.h"

int main(void)
{
    UmiWorkbenchContextEventFilter record;
    uint64_t first_hash;

    umi_workbench_context_event_filter_init(
        &record,
        "filter-record");
    assert(umi_workbench_context_event_filter_validate(
        &record) == UMI_STATUS_OK);

    assert(umi_workbench_context_event_filter_set_source(
        &record,
        "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_filter_set_subject(
        &record,
        "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_filter_set_group(
        &record,
        "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_filter_set_label(
        &record,
        "label") == UMI_STATUS_OK);

    record.event_kind =
        UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.priority =
        UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    record.state =
        UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;

    first_hash = umi_workbench_context_event_filter_hash(
        &record);
    assert(first_hash != 0U);
    umi_workbench_context_event_filter_touch(
        &record,
        7U,
        1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.subject_id, "subject") == 0);
    return 0;
}
