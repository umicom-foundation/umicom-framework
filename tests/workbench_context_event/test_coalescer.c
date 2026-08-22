/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_coalescer.c
 *
 * PURPOSE:
 *   Verify immediate duplicate editor events are coalesced.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_event/coalescer.h"

int main(void)
{
    UmiWorkbenchContextEventCoalescer coalescer;
    UmiWorkbenchContextEvent event;
    umi_workbench_context_event_coalescer_init(
        &coalescer,
        UMI_WORKBENCH_CONTEXT_EVENT_COALESCE_BY_SUBJECT,
        40U);
    umi_workbench_context_event_init(
        &event,
        UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET,
        "event");
    assert(umi_workbench_context_event_copy_text(
        event.application_id, sizeof(event.application_id),
        "studio") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        event.panel_id, sizeof(event.panel_id),
        "editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        event.subject_id, sizeof(event.subject_id),
        "main.c") == UMI_STATUS_OK);
    event.timestamp_ms = 100U;
    event.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    (void)umi_workbench_context_event_refresh_hash(&event);
    assert(umi_workbench_context_event_coalescer_accept(
        &coalescer, &event));
    event.timestamp_ms = 120U;
    assert(!umi_workbench_context_event_coalescer_accept(
        &coalescer, &event));
    assert(coalescer.coalesced_count == 1U);

    return 0;
}
