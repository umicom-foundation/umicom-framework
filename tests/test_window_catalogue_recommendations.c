/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_window_catalogue_recommendations.c
 *
 * PURPOSE:
 *   Verify the reusable New Window catalogue ranks recent tools, searches
 *   human-readable metadata and reports singleton tools already in a layout.
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

#include "umicom/ui/window_catalogue.h"

int main(void)
{
    UmiUiWindowCatalogue catalogue;
    UmiUiWindowDescriptor terminal = {
        "terminal", "Terminal", "Run local development commands", "terminal",
        UMI_UI_WINDOW_CATEGORY_DEVELOPMENT, true, 0.50, 0.28
    };
    UmiUiWindowDescriptor profiler = {
        "profiler", "Profiler", "Inspect CPU and memory use", "speedometer",
        UMI_UI_WINDOW_CATEGORY_OPERATIONS, false, 0.50, 0.50
    };
    UmiUiWorkspaceLayout layout;
    UmiUiWorkspaceWindow open_profiler = {
        "profiler", "Profiler", "profiler", "right",
        0.76, 0.0, 0.24, 0.72, true, false, false, true, 0
    };
    UmiUiWindowCatalogueQuery query;
    UmiUiWindowCatalogueResult result;

    umi_ui_window_catalogue_init(&catalogue);
    assert(umi_ui_window_catalogue_register(&catalogue, &terminal) ==
           UMI_STATUS_OK);
    assert(umi_ui_window_catalogue_register(&catalogue, &profiler) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_init(&layout, "develop", "Develop") ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&layout, false) == UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_add_window(&layout, &open_profiler) ==
           UMI_STATUS_OK);
    assert(umi_ui_workspace_layout_set_locked(&layout, true) == UMI_STATUS_OK);
    assert(umi_ui_window_catalogue_record_open(
               &catalogue, "terminal", UINT64_C(1200)) == UMI_STATUS_OK);

    query = umi_ui_window_catalogue_query_default();
    assert(umi_ui_window_catalogue_query(
               &catalogue, &layout, &query, &result) == UMI_STATUS_OK);
    assert(result.count == 2U);
    assert(strcmp(result.items[0].descriptor->tool_id, "terminal") == 0);
    assert(result.items[0].recent);
    assert(result.items[0].can_open);
    assert(strcmp(result.items[1].descriptor->tool_id, "profiler") == 0);
    assert(!result.items[1].can_open);
    assert(result.items[1].open_instance_count == 1U);

    (void)strcpy(query.text, "memory");
    assert(umi_ui_window_catalogue_query(
               &catalogue, &layout, &query, &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(strcmp(result.items[0].descriptor->tool_id, "profiler") == 0);
    return 0;
}
