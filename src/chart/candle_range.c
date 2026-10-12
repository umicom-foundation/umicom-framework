/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/candle_range.c
 * PURPOSE: Calculate bounded candle statistics without interpolating market observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/candle_range.h"
#include <math.h>

UmiStatus UmiChartCandleRangeAnalyze(const UmiChartCandle *candles, size_t count, size_t first,
                                     size_t last, UmiChartCandleRange *out) {
  if (out == NULL || (count != 0U && candles == NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_CHART_MAX_POINTS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (count == 0U)
    return UMI_STATUS_NOT_FOUND;
  if (first >= count || last >= count)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Validate all supplied observations, so selecting a smaller interval cannot
   * hide a corrupt record elsewhere in the same capture. */
  for (size_t index = 0U; index < count; ++index) {
    if (umi_chart_candle_validate(&candles[index]) != UMI_STATUS_OK ||
        (index != 0U && candles[index].time_ms <= candles[index - 1U].time_ms))
      return UMI_STATUS_INVALID_ARGUMENT;
  }
  if (first > last) {
    size_t swap = first;
    first = last;
    last = swap;
  }
  UmiChartCandleRange result = {0};
  result.first_index = first;
  result.last_index = last;
  result.count = last - first + 1U;
  result.first_time_ms = candles[first].time_ms;
  result.last_time_ms = candles[last].time_ms;
  /* Ordering is established above. Unsigned subtraction represents the full
   * INT64_MIN..INT64_MAX span without signed overflow. */
  result.elapsed_ms = (uint64_t)result.last_time_ms - (uint64_t)result.first_time_ms;
  result.open = candles[first].open;
  result.close = candles[last].close;
  result.high = candles[first].high;
  result.low = candles[first].low;
  for (size_t index = first; index <= last; ++index) {
    const UmiChartCandle *candle = &candles[index];
    result.high = fmax(result.high, candle->high);
    result.low = fmin(result.low, candle->low);
    result.volume += candle->volume;
    if (!isfinite(result.volume))
      return UMI_STATUS_UNAVAILABLE;
    if (candle->close > candle->open)
      ++result.rising;
    else if (candle->close < candle->open)
      ++result.falling;
    else
      ++result.unchanged;
    if (index != first) {
      uint64_t step = (uint64_t)candle->time_ms - (uint64_t)candles[index - 1U].time_ms;
      if (step > result.largest_step_ms)
        result.largest_step_ms = step;
    }
  }
  result.change = result.close - result.open;
  result.price_range = result.high - result.low;
  if (!isfinite(result.change) || !isfinite(result.price_range))
    return UMI_STATUS_UNAVAILABLE;
  /* Zero and negative bases do not have the usual financial percentage
   * interpretation. Keep the absolute change, but label percent unavailable.
   * Divide before multiplying so a representable percentage survives large prices. */
  if (result.open > 0.0) {
    double percent = (result.change / result.open) * 100.0;
    if (isfinite(percent)) {
      result.percent_change = percent;
      result.has_percent_change = 1;
    }
  }
  *out = result;
  return UMI_STATUS_OK;
}
