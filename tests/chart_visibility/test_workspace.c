/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_visibility/test_workspace.c
 * PURPOSE: Check visibility in the canonical trading workspace without changing orders.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading/chart_persistence.h"
#include "umicom/trading_ui/chart_scene.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)

/* Use the same workspace that owns two simulation orders and its draft. */
/* Drive one registered scenario through the public feature owners. */
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before, after; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *id = before.selected_instrument_id;
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "liquidity-zone", (UmiChartPoint){60000, 100}, (UmiChartPoint){120000, 105}));
    UmiChartDrawingSnapshot drawing, actual; OK(umi_chart_drawing_registry_at(registry, 0, &drawing));
    if (strcmp(name, "guards") == 0) {
        UmiInstrument other = test_instrument(); size_t changed = 99U;
        CHECK(UmiTradingWorkspaceSetChartDrawingHidden(f.workspace, other.instrument_id.value, drawing.id, drawing.revision, 1) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceSetChartDrawingsHidden(f.workspace, other.instrument_id.value, umi_chart_drawing_registry_revision(registry), 1, &changed) == UMI_STATUS_INVALID_STATE && changed == 99U);
        CHECK(UmiTradingWorkspaceSetChartDrawingHidden(f.workspace, id, drawing.id, drawing.revision - 1U, 1) == UMI_STATUS_BUSY);
        OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.visibility_flags == 0U && actual.revision == drawing.revision);
    } else if (strcmp(name, "persistence") == 0 || strcmp(name, "stale-preview") == 0) {
        UmiDataServer *server = NULL; UmiTradingChartPersistence *service = NULL;
        OK(umi_data_server_create_memory(&server)); OK(UmiTradingChartPersistenceCreate(f.workspace, &service));
        OK(UmiTradingChartPersistenceBind(service, server, "visibility"));
        OK(UmiTradingWorkspaceSetChartDrawingHidden(f.workspace, id, drawing.id, drawing.revision, 1));
        UmiChartCheckpointReport report; OK(UmiTradingChartPersistenceSave(service, id, 1000U, &report));
        UmiTradingChartPreview preview; OK(UmiTradingChartPersistencePreview(service, id, &preview));
        OK(umi_chart_drawing_registry_at(registry, 0, &actual));
        OK(UmiTradingWorkspaceSetChartDrawingHidden(f.workspace, id, drawing.id, actual.revision, 0));
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        if (strcmp(name, "stale-preview") == 0) {
            OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.visibility_flags == 0U);
        } else {
            OK(UmiTradingChartPersistencePreview(service, id, &preview));
            OK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id));
            OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.visibility_flags == UMI_CHART_DRAWING_VISIBILITY_HIDDEN);
        }
        UmiTradingChartPersistenceDestroy(service); umi_data_server_destroy(server);
    } else if (strcmp(name, "scene") == 0) {
        UmiTradingMarketSnapshot market; OK(umi_trading_workspace_selected_market(f.workspace, &market));
        for (int i = 0; i < 4; ++i) {
            UmiBar bar = {0}; bar.instrument = market.instrument; bar.start_time_ms = (i + 1) * 60000;
            bar.end_time_ms = bar.start_time_ms + 59999; bar.open = 100; bar.close = 101; bar.low = 90; bar.high = 110; bar.volume = 100;
            OK(umi_trading_workspace_update_bar(f.workspace, &bar, 100));
        }
        UmiTradingChartSceneInfo info; UmiChartRenderScene *scene = NULL;
        OK(UmiTradingChartBuildScene(f.workspace, &info, &scene)); size_t shown = umi_chart_render_scene_count(scene);
        umi_chart_render_scene_destroy(scene);
        OK(UmiTradingWorkspaceSetChartDrawingHidden(f.workspace, id, drawing.id, drawing.revision, 1));
        OK(UmiTradingChartBuildScene(f.workspace, &info, &scene)); CHECK(shown == umi_chart_render_scene_count(scene) + 2U);
        umi_chart_render_scene_destroy(scene);
    } else if (strcmp(name, "noop") == 0 || strcmp(name, "pane") == 0) {
        UmiTradingWorkspaceSnapshot start; OK(umi_trading_workspace_snapshot(f.workspace, &start));
        size_t changed = 99U;
        int hide = strcmp(name, "pane") == 0;
        OK(UmiTradingWorkspaceSetChartDrawingsHidden(f.workspace, id, umi_chart_drawing_registry_revision(registry), hide, &changed));
        CHECK(changed == (hide ? 1U : 0U)); OK(umi_trading_workspace_snapshot(f.workspace, &after));
        CHECK(after.revision == start.revision + (hide ? 1U : 0U));
        OK(umi_chart_drawing_registry_at(registry, 0, &actual)); CHECK(actual.visibility_flags == (hide ? 1U : 0U));
    } else return 2;
    OK(umi_trading_workspace_snapshot(f.workspace, &after));
    CHECK(after.order_count == before.order_count && after.environment == before.environment && after.live_armed == before.live_armed);
    CHECK(strcmp(after.selected_instrument_id, before.selected_instrument_id) == 0 && strcmp(after.selected_order_id, before.selected_order_id) == 0);
    CHECK(memcmp(&after.draft_order, &before.draft_order, sizeof before.draft_order) == 0);
    umi_trading_workspace_destroy(f.workspace); return 0;
}
