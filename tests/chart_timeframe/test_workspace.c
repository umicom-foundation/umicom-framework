/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_workspace.c
 * PURPOSE: Check projections, instrument guards and navigation without changing trading state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before; OK(umi_trading_workspace_snapshot(f.workspace, &before));
    const char *id = before.selected_instrument_id;
    TimeframeBars(&f, strcmp(name, "empty") == 0 ? 0U : 40U);
    OK(umi_trading_workspace_snapshot(f.workspace, &before));
    UmiChartNavigation navigation = {0}; UmiChartCandle candles[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
    UmiChartTimeframeSummary summary = {0};
    if (strcmp(name, "stale") == 0) {
        UmiInstrument other = test_instrument();
        CHECK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, other.instrument_id.value, 300000U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceBuildChartCandles(f.workspace, other.instrument_id.value, candles, 40U, &summary) == UMI_STATUS_INVALID_STATE);
        OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation)); CHECK(navigation.interval_ms == 0U);
    } else if (strcmp(name, "unsupported") == 0) {
        CHECK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 120000U) == UMI_STATUS_INVALID_ARGUMENT);
        OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation)); CHECK(navigation.interval_ms == 0U);
    } else if (strcmp(name, "crossing") == 0) {
        UmiTradingMarketSnapshot market; OK(umi_trading_workspace_selected_market(f.workspace, &market));
        UmiBar bar = {0}; bar.instrument = market.instrument; bar.start_time_ms = 2400000; bar.end_time_ms = 2760001;
        bar.open = 100; bar.high = 102; bar.low = 99; bar.close = 101; bar.volume = 10;
        OK(umi_trading_workspace_update_bar(f.workspace, &bar, 100));
        CHECK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 300000U) == UMI_STATUS_UNAVAILABLE);
        OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation)); CHECK(navigation.interval_ms == 0U);
    } else {
        OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 300000U));
        OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation)); CHECK(navigation.interval_ms == 300000U);
        OK(UmiTradingWorkspaceBuildChartCandles(f.workspace, id, candles, UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, &summary));
        if (strcmp(name, "empty") == 0) {
            CHECK(summary.source_count == 0U && summary.candle_count == 0U);
        } else {
            CHECK(summary.source_count == 40U && summary.candle_count == 8U);
            CHECK(candles[0].open == 100 && candles[0].high == 106 && candles[0].low == 99 && candles[0].close == 105 && candles[0].volume == 60);
        }
        if (strcmp(name, "navigation") == 0) {
            OK(UmiChartNavigationZoom(&navigation, candles, summary.candle_count, 1));
            CHECK(navigation.visible_bars == 6U && navigation.interval_ms == 300000U);
            OK(UmiChartNavigationPan(&navigation, candles, summary.candle_count, -2));
            CHECK(navigation.pinned && navigation.anchor_ms == 1500000);
            OK(UmiTradingWorkspaceSetChartNavigation(f.workspace, id, &navigation));
            OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 60000U));
            OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation));
            CHECK(navigation.visible_bars == 6U && navigation.pinned && navigation.anchor_ms == 1500000 && navigation.interval_ms == 60000U);
        } else if (strcmp(name, "instrument") == 0) {
            UmiInstrument other = test_instrument(); OK(umi_trading_workspace_select_instrument(f.workspace, other.instrument_id.value));
            OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, other.instrument_id.value, &navigation)); CHECK(navigation.interval_ms == 0U);
            OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, other.instrument_id.value, 3600000U));
            OK(umi_trading_workspace_select_instrument(f.workspace, id));
            OK(UmiTradingWorkspaceGetChartNavigation(f.workspace, id, &navigation)); CHECK(navigation.interval_ms == 300000U);
            /* Instrument selection intentionally updates the draft; compare only chart evidence in this branch. */
            umi_trading_workspace_destroy(f.workspace); return 0;
        } else if (strcmp(name, "source") == 0) {
            OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, id, 0U));
            OK(UmiTradingWorkspaceBuildChartCandles(f.workspace, id, candles, 40U, &summary)); CHECK(summary.candle_count == 40U);
            for (size_t i = 0; i < 40U; ++i) {
                UmiBar bar; OK(umi_trading_workspace_selected_bar_at(f.workspace, i, &bar));
                CHECK(bar.start_time_ms == (int64_t)i * 60000 && bar.end_time_ms == bar.start_time_ms + 59999);
                CHECK(bar.open == 100 + (double)i && bar.close == 101 + (double)i && bar.volume == 10 + (double)i);
                CHECK(candles[i].time_ms == bar.start_time_ms && candles[i].close == bar.close);
            }
        } else CHECK(strcmp(name, "aggregate") == 0 || strcmp(name, "empty") == 0);
    }
    SameTrading(f.workspace, &before); umi_trading_workspace_destroy(f.workspace); return 0;
}
