/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/test_trading.c
 * PURPOSE: Verify reviewed restores preserve trading state and reject stale or unsupported content.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading/chart_persistence.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); ReviewFixture fixture; ReviewFixtureInit(&fixture);
    UmiTradingWorkspace *workspace = fixture.workspace;
    UmiTradingMarketSnapshot market; CHECK(umi_trading_workspace_selected_market(workspace, &market) == UMI_STATUS_OK);
    const char *id = market.instrument.instrument_id.value;
    UmiDataServer *server = NULL; CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiTradingChartPersistence *service = NULL; CHECK(UmiTradingChartPersistenceCreate(workspace, &service) == UMI_STATUS_OK);
    CHECK(UmiTradingChartPersistenceBind(service, server, "local") == UMI_STATUS_OK);
    UmiChartPoint a = {60000, 100}, b = {120000, 110};
    CHECK(UmiTradingWorkspaceAddChartDrawing(workspace, id, "trend", a, b) == UMI_STATUS_OK);
/* The appended timeframe defaults to source bars. This explicit zero retains the fixture semantics when all consumers are rebuilt. The previous implementation remains for engineering review. */
#if 0
    UmiChartNavigation nav = {20U, 120000, 1}, latest = {0};
#endif
    UmiChartNavigation nav = {20U, 120000, 1, 0U}, latest = {0};
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, id, &nav) == UMI_STATUS_OK);
    UmiChartCheckpointReport report;
    CHECK(UmiTradingChartPersistenceSave(service, id, 1000U, &report) == UMI_STATUS_OK && report.storage_revision == 1U);
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, id, &latest) == UMI_STATUS_OK);
    UmiTradingChartPreview preview;
    CHECK(UmiTradingChartPersistencePreview(service, id, &preview) == UMI_STATUS_OK);
    CHECK(preview.saved.navigation.visible_bars == 20U && preview.current.navigation.visible_bars == 0U);
    UmiTradingWorkspaceSnapshot before, after; CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    if (strcmp(argv[1], "restore") == 0) {
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_OK);
        CHECK(UmiTradingWorkspaceGetChartNavigation(workspace, id, &latest) == UMI_STATUS_OK && latest.visible_bars == 20U);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(argv[1], "stale-view") == 0) {
        nav.visible_bars = 8U; CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, id, &nav) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceGetChartNavigation(workspace, id, &latest) == UMI_STATUS_OK && latest.visible_bars == 8U);
    } else if (strcmp(argv[1], "stale-drawing") == 0) {
        CHECK(UmiTradingWorkspaceAddChartDrawing(workspace, id, "support", a, a) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_count(umi_chart_workspace_drawings(umi_trading_workspace_charts(workspace))) == 2U);
    } else if (strcmp(argv[1], "stale-storage") == 0) {
        UmiChartDocument *other = Document(id, 222);
        CHECK(UmiChartCheckpointSave(server, "local", other, 1U, 2000U, &report) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingChartPersistenceSave(service, id, 3000U, &report) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingChartPersistencePreview(service, id, &preview) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceSave(service, id, 3000U, &report) == UMI_STATUS_OK && report.storage_revision == 3U);
        UmiChartDocumentDestroy(other);
    } else if (strcmp(argv[1], "preview-identity") == 0) {
        uint64_t first = preview.preview_id;
        CHECK(UmiTradingChartPersistencePreview(service, id, &preview) == UMI_STATUS_OK && preview.preview_id != first);
        CHECK(UmiTradingChartPersistenceRestore(service, id, first) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "new-session") == 0) {
        UmiTradingChartPersistenceDestroy(service); service = NULL;
        CHECK(UmiTradingChartPersistenceCreate(workspace, &service) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceBind(service, server, "local") == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceSave(service, id, 2000U, &report) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingChartPersistencePreview(service, id, &preview) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceSave(service, id, 2000U, &report) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "rebind") == 0) {
        CHECK(UmiTradingChartPersistenceBind(service, server, "second.profile") == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingChartPersistencePreview(service, id, &preview) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiTradingChartPersistenceSave(service, id, 2000U, &report) == UMI_STATUS_OK && report.storage_revision == 1U);
        CHECK(UmiTradingChartPersistenceBind(service, NULL, NULL) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistenceSave(service, id, 3000U, &report) == UMI_STATUS_UNAVAILABLE);
    } else if (strcmp(argv[1], "unsupported") == 0) {
        UmiChartDrawingSnapshot drawing = Drawing("unsupported", id); strcpy(drawing.tool, "future.tool");
        UmiChartDocument *other = NULL;
        CHECK(UmiChartDocumentCreate(id, &latest, &drawing, 1U, 0U, &other) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(server, "local", other, 1U, 2000U, &report) == UMI_STATUS_OK);
        CHECK(UmiTradingChartPersistencePreview(service, id, &preview) == UMI_STATUS_UNAVAILABLE);
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
        UmiChartDocumentDestroy(other);
    } else {
        CHECK(strcmp(argv[1], "restored-identity") == 0);
        UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(umi_trading_workspace_charts(workspace));
        uint64_t revision = umi_chart_drawing_registry_revision(registry);
        UmiChartDrawingSnapshot drawing = Drawing("placeholder", id);
        (void)snprintf(drawing.id, sizeof(drawing.id), "trading-drawing-%llu", (unsigned long long)(revision + 2U));
        UmiChartDocument *other = NULL; CHECK(UmiChartDocumentCreate(id, &latest, &drawing, 1U, 0U, &other) == UMI_STATUS_OK);
        CHECK(UmiTradingWorkspaceRestoreChart(workspace, other, revision, &latest) == UMI_STATUS_OK);
        CHECK(UmiTradingWorkspaceAddChartDrawing(workspace, id, "support", a, a) == UMI_STATUS_OK);
        CHECK(umi_chart_drawing_registry_count(registry) == 2U);
        UmiChartDocumentDestroy(other);
    }
    CHECK(umi_trading_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
    CHECK(after.order_count == before.order_count && after.environment == before.environment && after.live_armed == before.live_armed);
    CHECK(strcmp(after.selected_instrument_id, before.selected_instrument_id) == 0 && strcmp(after.selected_order_id, before.selected_order_id) == 0);
    CHECK(memcmp(&before.draft_order, &after.draft_order, sizeof(before.draft_order)) == 0);
    UmiTradingChartPersistenceDestroy(service); umi_data_server_destroy(server); umi_trading_workspace_destroy(workspace); return 0;
}
