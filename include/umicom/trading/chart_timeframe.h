/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/chart_timeframe.h
 * PURPOSE: Project chart candles from retained provider bars without mutating financial state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TRADING_CHART_TIMEFRAME_H
#define UMICOM_TRADING_CHART_TIMEFRAME_H
#include "umicom/trading/workspace.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Build candles for the selected instrument and its retained interval setting.
 * Owner-thread use; no provider request, synthetic gaps, order or quote writes.
 * Failure leaves caller arrays and summary unchanged. Empty history is OK. */
UmiStatus UmiTradingWorkspaceBuildChartCandles(const UmiTradingWorkspace *workspace,
    const char *instrumentId, UmiChartCandle *out, size_t capacity, UmiChartTimeframeSummary *summary);
/** Set an interval only after current retained data can be aggregated. Preserve
 * a pinned time and bar-count zoom; a different interval counts displayed
 * aggregate candles. A repeated setting is a no-op. No automatic storage write. */
UmiStatus UmiTradingWorkspaceSetChartTimeframe(UmiTradingWorkspace *workspace,
    const char *instrumentId, uint32_t intervalMs);
#ifdef __cplusplus
}
#endif
#endif
