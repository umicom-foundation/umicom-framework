/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_end_to_end_trading.c
 *
 * PURPOSE:
 *   Exercise TWS-inspired trading mosaic creation, context groups, browser
 *   preview and responsive projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchDesignerLayoutFactoryRequest request =
        umi_workbench_designer_layout_factory_request_default();
    UmiWorkbenchDesignerPalette palette;
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerBrowserPreview preview;
    UmiWorkbenchDesignerResponsivePreview responsive;
    const UmiWorkbenchDesignerResponsiveProfile *profile;
    UmiWorkbenchLayoutDocument mobile;
    UmiWorkbenchLayoutNode *watchlist;
    UmiWorkbenchLayoutNode *chart;

    request.profile = UMI_WORKBENCH_DESIGNER_LAYOUT_TRADING;
    TEST_REQUIRE_STATUS(test_copy_text(
        request.layout_id, sizeof(request.layout_id), "layout.trading.mosaic"));
    TEST_REQUIRE_STATUS(test_copy_text(
        request.name, sizeof(request.name), "Trading Mosaic"));
    umi_workbench_designer_palette_init(&palette);
    TEST_REQUIRE_STATUS(umi_workbench_designer_palette_seed_framework(&palette));
    TEST_REQUIRE_STATUS(umi_workbench_designer_layout_factory_create(
        &request, &palette, &document));
    watchlist = umi_workbench_layout_document_find_node_mutable(
        &document, "watchlist");
    chart = umi_workbench_layout_document_find_node_mutable(
        &document, "chart");
    TEST_REQUIRE(watchlist != NULL && chart != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_context_group(
        watchlist, "context.instrument.red"));
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_context_group(
        chart, "context.instrument.red"));
    TEST_REQUIRE(strcmp(watchlist->context_group_id,
                        chart->context_group_id) == 0);
    umi_workbench_designer_browser_preview_init(&preview);
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_preview_build(
        &preview, &document, (UmiWorkbenchDesignerSize){320.0, 200.0}));
    TEST_REQUIRE(preview.panel_count >= 6U);
    umi_workbench_designer_responsive_preview_init(&responsive);
    TEST_REQUIRE_STATUS(umi_workbench_designer_responsive_preview_seed(
        &responsive));
    TEST_REQUIRE_STATUS(umi_workbench_designer_responsive_preview_activate(
        &responsive, "tablet-landscape"));
    profile = umi_workbench_designer_responsive_preview_active(&responsive);
    TEST_REQUIRE(profile != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_designer_responsive_preview_apply(
        profile, &document, &mobile));
    TEST_REQUIRE(mobile.node_count == document.node_count);
    return 0;
}
