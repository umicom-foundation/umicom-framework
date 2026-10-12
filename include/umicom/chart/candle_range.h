/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/candle_range.h
 * PURPOSE: Measure an inclusive range of observed candles for every chart frontend.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_CANDLE_RANGE_H
#define UMICOM_CHART_CANDLE_RANGE_H
#include "umicom/chart/candle.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Copied statistics in chronological order, independent of click direction.
 * Volume keeps the provider's units; it is not money or a liquidity guarantee.
 * elapsed_ms measures opening time to opening time, excluding the final candle's
 * duration. largest_step_ms describes observations, not missing market data. */
typedef struct UmiChartCandleRange {
  size_t first_index, last_index, count;
  size_t rising, falling, unchanged;
  int64_t first_time_ms, last_time_ms;
  uint64_t elapsed_ms, largest_step_ms;
  double open, close, high, low, volume;
  double change, price_range, percent_change;
  int has_percent_change;
} UmiChartCandleRange;

/** Analyze inclusive candle indices, accepting either endpoint order.
 * Validate the complete input before publishing a result. Prices may be zero or
 * negative. A percentage is available only for a strictly positive first open
 * and a finite calculation. Other numeric overflow returns UNAVAILABLE.
 * Empty input returns NOT_FOUND; more than UMI_CHART_MAX_POINTS returns
 * CAPACITY_EXCEEDED. Bounds, invalid OHLCV and duplicate/descending times return
 * INVALID_ARGUMENT. Every failure leaves out unchanged. No caller storage is
 * retained or changed, and this function never requests data or places orders. */
UmiStatus UmiChartCandleRangeAnalyze(const UmiChartCandle *candles, size_t count, size_t first,
                                     size_t last, UmiChartCandleRange *out);

#ifdef __cplusplus
}
#endif
#endif
