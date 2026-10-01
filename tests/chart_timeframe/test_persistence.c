/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_persistence.c
 * PURPOSE: Verify saved timeframes, review invalidation and no-op stability with real chart persistence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; ReviewFixture f; ReviewFixtureInit(&f); TimeframeBars(&f, 40U);
    UmiTradingWorkspaceSnapshot before; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *id = before.selected_instrument_id; UmiDataServer *server = NULL; OK(umi_data_server_create_memory(&server));
    UmiTradingChartPersistence *service = NULL; OK(UmiTradingChartPersistenceCreate(f.workspace, &service));
    OK(UmiTradingChartPersistenceBind(service, server, "timeframes"));
    OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 300000U));
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace, id, "trend", (UmiChartPoint){60000,100}, (UmiChartPoint){120000,110}));
    UmiChartCheckpointReport report; OK(UmiTradingChartPersistenceSave(service, id, 1000U, &report));
    OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 0U));
    UmiTradingChartPreview preview; OK(UmiTradingChartPersistencePreview(service, id, &preview));
    CHECK(preview.saved.navigation.interval_ms == 300000U && preview.current.navigation.interval_ms == 0U);
    if (strcmp(name, "stale") == 0) {
        OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 60000U));
        CHECK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id) == UMI_STATUS_INVALID_STATE);
    } else {
        if (strcmp(name, "noop") == 0) {
            UmiTradingWorkspaceSnapshot first, second; OK(umi_trading_workspace_snapshot(f.workspace, &first));
            OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 0U));
            OK(umi_trading_workspace_snapshot(f.workspace, &second)); CHECK(first.revision == second.revision);
        }
        else CHECK(strcmp(name, "restore") == 0);
        OK(UmiTradingChartPersistenceRestore(service, id, preview.preview_id));
    }
    UmiChartNavigation navigation; OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation));
    CHECK(navigation.interval_ms == (strcmp(name,"stale")==0 ? 60000U : 300000U));
    UmiChartDrawingSnapshot drawing; OK(umi_chart_drawing_registry_at(umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace)),0U,&drawing));
    CHECK(drawing.time1 == 60000 && drawing.time2 == 120000 && drawing.value1 == 100 && drawing.value2 == 110);
    SameTrading(f.workspace, &before); UmiTradingChartPersistenceDestroy(service); umi_data_server_destroy(server);
    umi_trading_workspace_destroy(f.workspace); return 0;
}
