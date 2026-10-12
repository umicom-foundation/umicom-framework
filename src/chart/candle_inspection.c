/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/candle_inspection.c
 * PURPOSE: Keep chart data selection deterministic for every native or web frontend.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/candle_inspection.h"

/* Unsigned subtraction after ordering represents even INT64_MIN..INT64_MAX.
 * Signed subtraction or an absolute-value call would overflow at those limits. */
static uint64_t TimeDistance(int64_t left, int64_t right) {
  return left >= right ? (uint64_t)left - (uint64_t)right : (uint64_t)right - (uint64_t)left;
}
UmiStatus UmiChartCandleInspect(const UmiChartCandle *candles, size_t count, size_t first,
                                size_t visible_count, int64_t requested_time_ms,
                                UmiChartCandleInspection *out) {
  if (out == NULL || (count != 0U && candles == NULL) || first > count ||
      visible_count > count - first)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (count > UMI_CHART_MAX_POINTS)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  /* Validate before publishing. A corrupt record elsewhere in this captured
   * sequence must not become apparently trustworthy by moving the pointer. */
  for (size_t index = 0U; index < count; ++index)
    if (umi_chart_candle_validate(&candles[index]) != UMI_STATUS_OK ||
        (index != 0U && candles[index].time_ms <= candles[index - 1U].time_ms))
      return UMI_STATUS_INVALID_ARGUMENT;
  if (visible_count == 0U)
    return UMI_STATUS_NOT_FOUND;
  size_t nearest = first;
  uint64_t distance = TimeDistance(candles[first].time_ms, requested_time_ms);
  for (size_t index = first + 1U; index < first + visible_count; ++index) {
    uint64_t next = TimeDistance(candles[index].time_ms, requested_time_ms);
    if (next < distance) {
      nearest = index;
      distance = next;
    }
  }
  /* The copied result survives the caller's next history refresh. No pointer
   * into a mutable provider cache escapes this function. */
  UmiChartCandleInspection result = {candles[nearest], nearest, requested_time_ms, distance};
  *out = result;
  return UMI_STATUS_OK;
}
