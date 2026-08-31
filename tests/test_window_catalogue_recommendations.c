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
    /* Named catalogue fields document the fixture and remain stable when the
     * Framework grows the reusable window descriptor. */
    UmiUiWindowDescriptor terminal = {
        .tool_id = "terminal",
        .title = "Terminal",
        .description = "Run local development commands",
        .icon_name = "terminal",
        .category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,
        .supports_multiple = true,
        .default_width = 0.50,
        .default_height = 0.28
    };
    UmiUiWindowDescriptor profiler = {
        .tool_id = "profiler",
        .title = "Profiler",
        .description = "Inspect CPU and memory use",
        .icon_name = "speedometer",
        .category = UMI_UI_WINDOW_CATEGORY_OPERATIONS,
        .supports_multiple = false,
        .default_width = 0.50,
        .default_height = 0.50
    };
    UmiUiWorkspaceLayout layout;
    UmiUiWorkspaceWindow open_profiler = {
        .window_id = "profiler",
        .title = "Profiler",
        .tool_id = "profiler",
        .group_id = "right",
        .x = 0.76,
        .y = 0.0,
        .width = 0.24,
        .height = 0.72,
        .visible = true,
        .floating = false,
        .maximised = false,
        .closable = true,
        .z_order = 0,
        .placement_id = "right",
        .stack_id = "right",
        .context_group_id = "",
        .pinned = true,
        .resizable = true
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
