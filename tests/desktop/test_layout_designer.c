/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_layout_designer.c
 *
 * PURPOSE:
 *   Implement the test layout designer behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Visual layout designer | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>

#include "umicom/desktop/layout_designer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopLayoutDesigner *designer = NULL;
    UmiDesktopLayoutDesignerSnapshot snapshot;
    const UmiDesktopWindow *chart;
    UmiDesktopRect original;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_create(runtime, &designer) ==
           UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_begin(
        designer, "trading", "trading-custom-b79", "Trading Custom") ==
        UMI_STATUS_OK);
    assert(strcmp(umi_desktop_runtime_layouts(runtime)->active_layout_id,
                  "trading-custom-b79") == 0);
    assert(umi_desktop_layout_designer_select(
        designer, "trading-chart") == UMI_STATUS_OK);
    chart = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "trading-chart");
    assert(chart != NULL);
    original = chart->bounds;
    assert(umi_desktop_layout_designer_dock_selected(
        designer, UMI_DESKTOP_DOCK_LEFT) == UMI_STATUS_OK);
    chart = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "trading-chart");
    assert(chart->dock_placement == UMI_DESKTOP_DOCK_LEFT);
    assert(chart->bounds.width == 960);
    assert(umi_desktop_layout_designer_snapshot(designer, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.active && snapshot.created_copy && snapshot.undo_count == 1U);
    assert(snapshot.dirty && snapshot.can_commit);
    assert(umi_desktop_layout_designer_undo(designer) == UMI_STATUS_OK);
    chart = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "trading-chart");
    assert(memcmp(&chart->bounds, &original, sizeof(original)) == 0);
    assert(umi_desktop_layout_designer_redo(designer) == UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_commit(designer) == UMI_STATUS_OK);
    assert(!umi_desktop_layout_tabs_find(
        umi_desktop_runtime_tabs(runtime), "trading-custom-b79")->dirty);
    assert(umi_desktop_layout_designer_begin(
        designer, "develop", "develop-cancel-b79", "Develop Cancel") ==
        UMI_STATUS_OK);
    assert(umi_desktop_layout_designer_cancel(designer) == UMI_STATUS_OK);
    assert(umi_desktop_layout_catalogue_find(
        umi_desktop_runtime_layouts(runtime), "develop-cancel-b79") == NULL);
    assert(strcmp(umi_desktop_runtime_layouts(runtime)->active_layout_id,
                  "develop") == 0);
    umi_desktop_layout_designer_destroy(designer);
    umi_desktop_runtime_destroy(runtime);
    umi_application_context_hub_destroy(hub);
    return 0;
}
