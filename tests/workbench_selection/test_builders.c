/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection/test_builders.c
 *
 * PURPOSE:
 *   Verify structured builders for Studio and trading selections.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection/builders.h"

int main(void)
{
    UmiWorkbenchSelection selection;

    assert(umi_workbench_selection_build_project(
        &selection, "project-selection", "studio",
        "studio.project-explorer", "workspace",
        "umicom", "C:/dev/umicom", "repo",
        "c", 10U) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_PROJECT);
    assert(selection.context_kind == UMI_CONTEXT_KIND_PROJECT);
    assert(umi_workbench_selection_find_field(
        &selection, "repository-id") != NULL);

    assert(umi_workbench_selection_build_diagnostic(
        &selection, "diagnostic-selection", "studio",
        "studio.problems", "workspace",
        "E001", "C:/dev/main.c", 10U, 4U,
        "error", "E001", "Example diagnostic",
        20U) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_DIAGNOSTIC);
    assert(selection.context_kind == UMI_CONTEXT_KIND_SOURCE_LOCATION);

    assert(umi_workbench_selection_build_instrument(
        &selection, "instrument-selection", "trader",
        "trader.watchlist", "ESU6", "ES", "CME",
        "USD", "future", "ESU6", 30U) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_INSTRUMENT);
    assert(selection.context_kind == UMI_CONTEXT_KIND_INSTRUMENT);

    assert(umi_workbench_selection_build_trade(
        &selection, "trade-selection", "tms",
        "tms.trade-list", "trade-1", "tms",
        "future", "book", "counterparty",
        7U, 40U) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_TRADE);
    assert(selection.unsigned_value == 7U);
    return 0;
}
