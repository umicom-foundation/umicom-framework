/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/chart_timeframe.c
 * PURPOSE: Keep interval aggregation shared between native navigation and scene rendering.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading/chart_timeframe.h"
#include <stdlib.h>
static UmiStatus Build(const UmiTradingWorkspace *workspace, uint32_t interval,
    UmiChartCandle *out, size_t capacity, UmiChartTimeframeSummary *summary)
{
    size_t count = umi_trading_workspace_selected_bar_count(workspace);
    if (count > UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY) return UMI_STATUS_INVALID_STATE;
    if (count == 0U) return UmiChartTimeframeAggregate(NULL, 0U, interval, out, capacity, summary);
    UmiChartObservedBar *source = calloc(count, sizeof *source);
    if (source == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < count; ++i) {
        UmiBar bar; status = umi_trading_workspace_selected_bar_at(workspace, i, &bar);
        if (status != UMI_STATUS_OK) break;
        source[i].candle = (UmiChartCandle){bar.start_time_ms, bar.open, bar.high, bar.low, bar.close, bar.volume};
        source[i].end_ms = bar.end_time_ms;
    }
    if (status == UMI_STATUS_OK) status = UmiChartTimeframeAggregate(source, count, interval, out, capacity, summary);
    free(source); return status;
}
UmiStatus UmiTradingWorkspaceBuildChartCandles(const UmiTradingWorkspace *workspace,
    const char *instrumentId, UmiChartCandle *out, size_t capacity, UmiChartTimeframeSummary *summary)
{
    UmiChartNavigation navigation;
    UmiStatus status = UmiTradingWorkspaceGetChartNavigation(workspace, instrumentId, &navigation);
    return status == UMI_STATUS_OK ? Build(workspace, navigation.interval_ms, out, capacity, summary) : status;
}
UmiStatus UmiTradingWorkspaceSetChartTimeframe(UmiTradingWorkspace *workspace, const char *instrumentId, uint32_t intervalMs)
{
    if (!UmiChartTimeframeValid(intervalMs)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartNavigation navigation;
    UmiStatus status = UmiTradingWorkspaceGetChartNavigation(workspace, instrumentId, &navigation);
    if (status != UMI_STATUS_OK || navigation.interval_ms == intervalMs) return status;
    UmiChartCandle *candles = calloc(UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, sizeof *candles);
    if (candles == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiChartTimeframeSummary summary;
    status = Build(workspace, intervalMs, candles, UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, &summary);
    free(candles);
    if (status == UMI_STATUS_OK) {
        navigation.interval_ms = intervalMs;
        status = UmiTradingWorkspaceSetChartNavigation(workspace, instrumentId, &navigation);
    }
    return status;
}
