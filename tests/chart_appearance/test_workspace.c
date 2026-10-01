/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_appearance/test_workspace.c
 * PURPOSE: Verify appearance composition, preserved order intent and persisted chart review conflicts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/chart/drawing_appearance.h"
#include "umicom/trading/chart_persistence.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before, after; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *pane = before.selected_instrument_id;
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, pane, "range", (UmiChartPoint){60000,100}, (UmiChartPoint){120000,105}));
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiChartDrawingSnapshot drawing, current; OK(umi_chart_drawing_registry_at(registry, 0, &drawing));
    UmiChartDrawingAppearance appearance = {51,170,255,25,30};
    if (strcmp(name, "apply") == 0) {
        OK(umi_trading_workspace_snapshot(f.workspace, &after));
        uint64_t originalRevision = after.revision;
        OK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace, pane, drawing.id, drawing.revision, &appearance));
        OK(umi_chart_drawing_registry_at(registry, 0, &current));
        CHECK(strcmp(current.style, "umi-drawing:1:33AAFF:25:30") == 0);
        OK(umi_trading_workspace_snapshot(f.workspace, &after));
        CHECK(after.revision == originalRevision + 1U);
        uint64_t version = after.revision;
        OK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace, pane, current.id, current.revision, &appearance));
        OK(umi_trading_workspace_snapshot(f.workspace, &after)); CHECK(after.revision == version);
    } else if (strcmp(name, "guards") == 0) {
        UmiInstrument other = test_instrument();
        CHECK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace, other.instrument_id.value, drawing.id, drawing.revision, &appearance) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace, pane, drawing.id, drawing.revision-1, &appearance) == UMI_STATUS_BUSY);
        CHECK(umi_chart_drawing_registry_revision(registry) == drawing.revision);
    } else if (strcmp(name, "persistence") == 0 || strcmp(name, "legacy-roundtrip") == 0) {
        int legacy = strcmp(name, "legacy-roundtrip") == 0;
        if (legacy) { strcpy(drawing.style, "third-party:keep-this"); OK(umi_chart_drawing_registry_upsert(registry, &drawing)); }
        else OK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace, pane, drawing.id, drawing.revision, &appearance));
        UmiDataServer *server = NULL; UmiTradingChartPersistence *service = NULL;
        OK(umi_data_server_create_memory(&server)); OK(UmiTradingChartPersistenceCreate(f.workspace, &service));
        OK(UmiTradingChartPersistenceBind(service, server, "appearance"));
        UmiChartCheckpointReport report; UmiTradingChartPreview preview;
        OK(UmiTradingChartPersistenceSave(service, pane, 1000, &report));
        OK(UmiTradingChartPersistencePreview(service, pane, &preview));
        OK(umi_chart_drawing_registry_at(registry, 0, &current));
        OK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace, pane, current.id, current.revision, NULL));
        CHECK(UmiTradingChartPersistenceRestore(service, pane, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        OK(UmiTradingChartPersistencePreview(service, pane, &preview));
        OK(UmiTradingChartPersistenceRestore(service, pane, preview.preview_id));
        OK(umi_chart_drawing_registry_at(registry, 0, &current));
        CHECK(strcmp(current.style, legacy ? "third-party:keep-this" : "umi-drawing:1:33AAFF:25:30") == 0);
        CHECK(current.time1 == drawing.time1 && current.value2 == drawing.value2);
        UmiTradingChartPersistenceDestroy(service); umi_data_server_destroy(server);
    } else return 2;
    OK(umi_trading_workspace_snapshot(f.workspace, &after));
    CHECK(after.order_count == before.order_count && after.live_armed == before.live_armed && after.environment == before.environment);
    CHECK(strcmp(after.selected_order_id, before.selected_order_id) == 0);
    CHECK(memcmp(&after.draft_order, &before.draft_order, sizeof before.draft_order) == 0);
    umi_trading_workspace_destroy(f.workspace); return 0;
}
