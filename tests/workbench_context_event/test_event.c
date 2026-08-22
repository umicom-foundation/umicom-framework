/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_event.c
 *
 * PURPOSE:
 *   Verify normalised event validation, metadata and hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_event/event.h"

int main(void)
{
    UmiWorkbenchContextEvent event;
    umi_workbench_context_event_init(
        &event,
        UMI_WORKBENCH_CONTEXT_EVENT_PROJECT_SELECTION,
        "event");
    assert(umi_workbench_context_event_copy_text(
        event.application_id, sizeof(event.application_id),
        "org.umicom.test") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        event.panel_id, sizeof(event.panel_id),
        "project") == UMI_STATUS_OK);
    event.context_kind = UMI_CONTEXT_KIND_PROJECT;
    assert(umi_workbench_context_event_add_metadata(
        &event, "language-id", "c") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_validate(
        &event) == UMI_STATUS_OK);
    assert(umi_workbench_context_event_refresh_hash(&event) != 0U);
    assert(umi_workbench_context_event_find_metadata(
        &event, "language-id") != NULL);

    return 0;
}
