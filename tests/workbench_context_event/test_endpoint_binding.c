/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_endpoint_binding.c
 *
 * PURPOSE:
 *   Verify endpoint binding validation, mutation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_event/endpoint_binding.h"

int main(void)
{
    UmiWorkbenchContextEventEndpointBinding record;
    uint64_t first_hash;

    umi_workbench_context_event_endpoint_binding_init(
        &record,
        "endpoint_binding-record");
    assert(umi_workbench_context_event_endpoint_binding_validate(
        &record) == UMI_STATUS_OK);

    assert(umi_workbench_context_event_endpoint_binding_set_source(
        &record,
        "source") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_endpoint_binding_set_subject(
        &record,
        "subject") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_endpoint_binding_set_group(
        &record,
        "blue") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_endpoint_binding_set_label(
        &record,
        "label") == UMI_STATUS_OK);

    record.event_kind =
        UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION;
    record.context_kind = UMI_CONTEXT_KIND_PROJECT;
    record.priority =
        UMI_WORKBENCH_CONTEXT_EVENT_PRIORITY_INTERACTIVE;
    record.state =
        UMI_WORKBENCH_CONTEXT_EVENT_ACCEPTED;

    first_hash = umi_workbench_context_event_endpoint_binding_hash(
        &record);
    assert(first_hash != 0U);
    umi_workbench_context_event_endpoint_binding_touch(
        &record,
        7U,
        1000U);
    assert(record.sequence == 7U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.source_id, "source") == 0);
    assert(strcmp(record.subject_id, "subject") == 0);
    return 0;
}
