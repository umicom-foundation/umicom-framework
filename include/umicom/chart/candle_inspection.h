/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/candle_inspection.h
 * PURPOSE: Select an observed candle for data inspection without inventing values between
 * observations. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_CANDLE_INSPECTION_H
#define UMICOM_CHART_CANDLE_INSPECTION_H
#include "umicom/chart/candle.h"
#ifdef __cplusplus
extern "C" {
#endif

/** A copied observation and its exact unsigned distance from the requested time.
 * index addresses the original input array. distance_ms is not a market-data
 * latency measurement: the requested time may lie anywhere on a chart. */
typedef struct UmiChartCandleInspection {
  UmiChartCandle candle;
  size_t index;
  int64_t requested_time_ms;
  uint64_t distance_ms;
} UmiChartCandleInspection;

/** Find the nearest observed candle within [first, first + visible_count).
 * Ties choose the earlier candle. The full input must contain finite valid
 * OHLCV values and strictly increasing timestamps, with at most
 * UMI_CHART_MAX_POINTS records. Prices may be zero or negative.
 *
 * A time outside the window chooses its nearest end candle. Hosts should
 * reject pointer positions outside their plot before calling. Empty windows
 * return NOT_FOUND; malformed bounds return INVALID_ARGUMENT. Every failure
 * leaves out unchanged. Input is read synchronously and is never sorted or
 * changed. This operation does not interpolate prices, infer missing bars,
 * change an order, or access a broker. */
UmiStatus UmiChartCandleInspect(const UmiChartCandle *candles, size_t count, size_t first,
                                size_t visible_count, int64_t requested_time_ms,
                                UmiChartCandleInspection *out);

#ifdef __cplusplus
}
#endif
#endif
