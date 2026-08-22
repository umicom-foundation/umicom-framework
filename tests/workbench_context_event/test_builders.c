/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_event/test_builders.c
 *
 * PURPOSE:
 *   Verify editor/project/trading event builders produce typed normalised events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/workbench_context_event/builders.h"

int main(void)
{
    UmiWorkbenchContextEvent event;
    assert(umi_workbench_context_event_build_project(
        &event, "project-event", "studio.project.selection",
        "org.umicom.studio", "studio.project-explorer",
        "workspace", "project", "C:/dev/project", "repo", "c",
        10U) == UMI_STATUS_OK);
    assert(event.context_kind == UMI_CONTEXT_KIND_PROJECT);
    assert(strcmp(event.subject_id, "project") == 0);
    assert(umi_workbench_context_event_build_editor_location(
        &event, "editor-event", "studio.editor.location",
        "org.umicom.studio", "studio.editor", "workspace",
        "C:/dev/main.c", "main", 4U, 2U, 0U,
        20U) == UMI_STATUS_OK);
    assert(event.context_kind == UMI_CONTEXT_KIND_SOURCE_LOCATION);
    assert(umi_workbench_context_event_build_instrument(
        &event, "instrument-event", "trader.watchlist",
        "org.umicom.trader", "trader.watchlist",
        "ESU6", "ES", "CME", "USD", "future", "ESU6",
        30U) == UMI_STATUS_OK);
    assert(event.context_kind == UMI_CONTEXT_KIND_INSTRUMENT);

    return 0;
}
