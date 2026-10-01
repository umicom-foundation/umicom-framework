/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_chart_workspace.c
 * PURPOSE: Exercise chart navigation, drawing isolation and non-submitting ticket preparation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "order_review_fixture.h"
#include "umicom/trading_ui/chart_scene.h"
#include <math.h>
#define CHECK REVIEW_CHECK
static void Candles(UmiChartCandle *candles, size_t count)
{
    for (size_t i = 0; i < count; ++i)
        candles[i] = (UmiChartCandle){60000 * (int64_t)(i + 1U), 100, 110, 90, 101, 50 + (double)i};
}
static void Bars(UmiTradingWorkspace *workspace)
{
    UmiTradingMarketSnapshot market;
    CHECK(umi_trading_workspace_selected_market(workspace, &market) == UMI_STATUS_OK);
    for (int i = 0; i < 40; ++i) {
        UmiBar bar = {0}; bar.instrument = market.instrument;
        bar.start_time_ms = 60000 * (i + 1); bar.end_time_ms = bar.start_time_ms + 59999;
        bar.open = 100; bar.high = 110; bar.low = 90; bar.close = 101; bar.volume = 100 + i;
        CHECK(umi_trading_workspace_update_bar(workspace, &bar, 100) == UMI_STATUS_OK);
    }
}
static int SceneContains(const UmiChartRenderScene *scene, const char *text)
{
    for (size_t i = 0; i < umi_chart_render_scene_count(scene); ++i) {
        UmiChartRenderCommand command;
        CHECK(umi_chart_render_scene_at(scene, i, &command) == UMI_STATUS_OK);
        if (command.kind == UMI_CHART_RENDER_TEXT && strstr(command.text, text) != NULL) return 1;
    }
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    UmiChartCandle candles[41]; Candles(candles, 41);
    UmiChartNavigation navigation = {0}; UmiChartWindow window;
    if (strcmp(argv[1], "navigation") == 0) {
        CHECK(UmiChartNavigationResolve(&navigation, candles, 40, &window) == UMI_STATUS_OK && window.count == 40 && window.first == 0);
        CHECK(UmiChartNavigationZoom(&navigation, candles, 40, 1) == UMI_STATUS_OK);
        CHECK(UmiChartNavigationResolve(&navigation, candles, 40, &window) == UMI_STATUS_OK && window.count == 30 && window.first == 10);
        CHECK(UmiChartNavigationPan(&navigation, candles, 40, -5) == UMI_STATUS_OK && navigation.pinned);
        CHECK(UmiChartNavigationResolve(&navigation, candles, 41, &window) == UMI_STATUS_OK && window.first == 5 && window.count == 30);
        CHECK(UmiChartNavigationPan(&navigation, candles, 41, 10000) == UMI_STATUS_OK && !navigation.pinned);
        CHECK(UmiChartNavigationZoom(&navigation, candles, 41, 32) == UMI_STATUS_OK && navigation.visible_bars == 4);
        CHECK(UmiChartNavigationZoom(&navigation, candles, 41, -32) == UMI_STATUS_OK && navigation.visible_bars == 41);
    } else if (strcmp(argv[1], "anchors") == 0) {
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
        navigation = (UmiChartNavigation){5, 60000, 1};
#endif
        navigation = (UmiChartNavigation){5, 60000, 1, 0U};
        CHECK(UmiChartNavigationResolve(&navigation, candles + 10, 30, &window) == UMI_STATUS_OK && window.first == 0 && window.count == 5);
        UmiChartNavigation saved = navigation;
        CHECK(UmiChartNavigationZoom(&navigation, candles, 41, 33) == UMI_STATUS_INVALID_ARGUMENT && navigation.visible_bars == saved.visible_bars);
        candles[2].time_ms = candles[1].time_ms;
        CHECK(UmiChartNavigationPan(&navigation, candles, 41, 1) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(navigation.anchor_ms == saved.anchor_ms);
    } else if (strcmp(argv[1], "coordinates") == 0) {
        UmiChartPlotViewport viewport = {{10, 20, 800, 400}, 1000, 9000, 50, 150};
        UmiChartPoint point = {17, 18};
        CHECK(UmiChartPlotUnmap(&viewport, 410, 220, &point) == UMI_STATUS_OK && point.time_ms == 5000 && point.value == 100);
        CHECK(UmiChartPlotUnmap(&viewport, 9, 220, &point) == UMI_STATUS_INVALID_ARGUMENT && point.value == 100);
        CHECK(UmiChartPlotUnmap(&viewport, NAN, 220, &point) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartPlotUnmap(&viewport, 810, 420, &point) == UMI_STATUS_OK && point.time_ms == 9000 && point.value == 50);
    } else {
        ReviewFixture f; ReviewFixtureInit(&f);
        UmiTradingWorkspaceSnapshot before, after;
        CHECK(umi_trading_workspace_snapshot(f.workspace, &before) == UMI_STATUS_OK);
        const char *id = before.selected_instrument_id;
        UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
        if (strcmp(argv[1], "drawings") == 0) {
            CHECK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "support", (UmiChartPoint){60000, 95}, (UmiChartPoint){60000, 95}) == UMI_STATUS_OK);
            UmiChartDrawingSnapshot drawing;
            CHECK(umi_chart_drawing_registry_at(registry, 0, &drawing) == UMI_STATUS_OK && strcmp(drawing.pane_id, id) == 0);
            CHECK(UmiTradingWorkspaceRemoveChartDrawing(f.workspace, id, drawing.id, drawing.revision + 1U) == UMI_STATUS_INVALID_STATE);
            drawing.locked = 1; CHECK(umi_chart_drawing_registry_upsert(registry, &drawing) == UMI_STATUS_OK);
            CHECK(umi_chart_drawing_registry_at(registry, 0, &drawing) == UMI_STATUS_OK);
            CHECK(UmiTradingWorkspaceRemoveChartDrawing(f.workspace, id, drawing.id, drawing.revision) == UMI_STATUS_INVALID_STATE);
            drawing.locked = 0; CHECK(umi_chart_drawing_registry_upsert(registry, &drawing) == UMI_STATUS_OK);
            CHECK(umi_chart_drawing_registry_at(registry, 0, &drawing) == UMI_STATUS_OK);
            CHECK(UmiTradingWorkspaceRemoveChartDrawing(f.workspace, id, drawing.id, drawing.revision) == UMI_STATUS_OK);
            CHECK(umi_chart_drawing_registry_count(registry) == 0);
        } else if (strcmp(argv[1], "ticket") == 0) {
            CHECK(UmiTradingUiControllerPrepareChartLimit(&f.controller, id, UMI_SIDE_SELL, 102.25) == UMI_STATUS_OK);
            CHECK(umi_trading_workspace_snapshot(f.workspace, &after) == UMI_STATUS_OK);
            CHECK(after.order_count == before.order_count && after.draft_order.side == UMI_SIDE_SELL);
            CHECK(after.draft_order.type == UMI_ORDER_LIMIT && after.draft_order.limit_price == 102.25 && !after.has_draft_risk);
            CHECK(after.draft_order.quantity == before.draft_order.quantity && after.draft_order.tif == before.draft_order.tif);
            CHECK(after.environment == before.environment && !after.live_armed);
            CHECK(UmiTradingWorkspacePrepareChartLimit(f.workspace, "old-symbol", UMI_SIDE_BUY, 100) == UMI_STATUS_INVALID_STATE);
            CHECK(UmiTradingWorkspacePrepareChartLimit(f.workspace, id, UMI_SIDE_BUY, NAN) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(umi_trading_workspace_snapshot(f.workspace, &before) == UMI_STATUS_OK && before.revision == after.revision);
        } else if (strcmp(argv[1], "isolation") == 0) {
            UmiInstrument first = test_instrument();
            navigation.visible_bars = 12;
            CHECK(UmiTradingWorkspaceSetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK);
            CHECK(umi_trading_workspace_select_instrument(f.workspace, first.instrument_id.value) == UMI_STATUS_OK);
            CHECK(UmiTradingWorkspaceGetChartNavigation(f.workspace, first.instrument_id.value, &navigation) == UMI_STATUS_OK && navigation.visible_bars == 0);
            CHECK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "support", (UmiChartPoint){60000, 95}, (UmiChartPoint){60000, 95}) == UMI_STATUS_INVALID_STATE);
            CHECK(umi_trading_workspace_select_instrument(f.workspace, id) == UMI_STATUS_OK);
            CHECK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK && navigation.visible_bars == 12);
        } else {
            CHECK(strcmp(argv[1], "scene") == 0);
            Bars(f.workspace);
            CHECK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "support", (UmiChartPoint){60000, 95}, (UmiChartPoint){60000, 95}) == UMI_STATUS_OK);
            CHECK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "trend", (UmiChartPoint){60000, 85}, (UmiChartPoint){2400000, 115}) == UMI_STATUS_OK);
            CHECK(umi_trading_workspace_set_chart_study(f.workspace, UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE, 5) == UMI_STATUS_OK);
            navigation.visible_bars = 10; CHECK(UmiTradingWorkspaceSetChartNavigation(f.workspace, id, &navigation) == UMI_STATUS_OK);
            UmiTradingChartSceneInfo info; UmiChartRenderScene *scene = NULL;
            CHECK(UmiTradingChartBuildScene(f.workspace, &info, &scene) == UMI_STATUS_OK);
            CHECK(info.window.count == 10 && info.window.first == 30 && info.retained_bars == 40);
            CHECK(SceneContains(scene, "Volume") && SceneContains(scene, "support 95") && SceneContains(scene, "DRAFT"));
            CHECK(SceneContains(scene, f.second) && !SceneContains(scene, f.first));
            umi_chart_render_scene_destroy(scene);
        }
        umi_trading_workspace_destroy(f.workspace);
    }
    return 0;
}
