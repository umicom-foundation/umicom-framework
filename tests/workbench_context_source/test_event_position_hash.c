/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_event_position_hash.c
 *
 * PURPOSE:
 *   Verify caret/range changes alter the normalised event content hash.
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
    UmiWorkbenchContextEvent first;
    UmiWorkbenchContextEvent second;
    umi_workbench_context_event_init(
        &first, UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET,
        "first");
    umi_workbench_context_event_init(
        &second, UMI_WORKBENCH_CONTEXT_EVENT_EDITOR_CARET,
        "second");
    assert(umi_workbench_context_event_copy_text(
        first.application_id, sizeof(first.application_id),
        "studio") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        second.application_id, sizeof(second.application_id),
        "studio") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        first.panel_id, sizeof(first.panel_id),
        "editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        second.panel_id, sizeof(second.panel_id),
        "editor") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        first.path, sizeof(first.path),
        "main.c") == UMI_STATUS_OK);
    assert(umi_workbench_context_event_copy_text(
        second.path, sizeof(second.path),
        "main.c") == UMI_STATUS_OK);
    first.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    second.context_kind = UMI_CONTEXT_KIND_SOURCE_LOCATION;
    first.line = 10U;
    second.line = 11U;
    assert(umi_workbench_context_event_refresh_hash(&first) !=
           umi_workbench_context_event_refresh_hash(&second));

    return 0;
}
