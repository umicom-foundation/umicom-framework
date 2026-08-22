/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_payload_builder.c
 *
 * PURPOSE:
 *   Verify canonical domain payload construction for Studio and Trader workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/payload_builder.h"

int main(void)
{
    UmiContextPayload source;
    UmiContextPayload instrument;
    UmiContextPayload project;
    UmiContextPayload selection;
    assert(umi_workbench_context_link_build_source_location(
        &source, "source", "studio", "editor", "workspace",
        "C:/dev/main.c", 10U, 4U) == UMI_STATUS_OK);
    assert(source.kind == UMI_CONTEXT_KIND_SOURCE_LOCATION);
    assert(umi_workbench_context_link_build_instrument(
        &instrument, "instrument", "trader", "watchlist",
        "ES-SEP", "ES", "CME") == UMI_STATUS_OK);
    assert(instrument.kind == UMI_CONTEXT_KIND_INSTRUMENT);
    assert(umi_workbench_context_link_build_project(
        &project, "project", "studio", "explorer",
        "umicom", "C:/dev/umicom", "c") == UMI_STATUS_OK);
    assert(project.kind == UMI_CONTEXT_KIND_PROJECT);
    assert(umi_workbench_context_link_build_selection(
        &selection, "selection", "studio", "problems",
        "problem-1", "diagnostic", "main.c:10") == UMI_STATUS_OK);
    assert(selection.kind == UMI_CONTEXT_KIND_SELECTION);
    return 0;
}
