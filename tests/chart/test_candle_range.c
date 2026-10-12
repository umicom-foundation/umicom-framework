/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_candle_range.c
 * PURPOSE: Check chronological measurements, numeric limits and unchanged outputs on rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/candle_range.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  UmiChartCandle candles[] = {
      {1000, 100, 110, 95, 105, 20}, {2000, 105, 108, 99, 101, 30}, {8000, 101, 112, 100, 101, 50}};
  UmiChartCandleRange range;
  memset(&range, 0x5a, sizeof(range));
  unsigned char previous[sizeof(range)];
  memcpy(previous, &range, sizeof(range));
  size_t first = 0U, last = 2U, count = 3U;
  const char *mode = argv[1];
  if (strcmp(mode, "reverse") == 0) {
    first = 2U;
    last = 0U;
  } else if (strcmp(mode, "single") == 0)
    first = last = 1U;
  else if (strcmp(mode, "zero-open") == 0)
    candles[0] = (UmiChartCandle){1000, 0, 110, 0, 105, 20};
  else if (strcmp(mode, "negative-open") == 0)
    candles[0] = (UmiChartCandle){1000, -100, 110, -110, 105, 20};
  else if (strcmp(mode, "extreme-time") == 0) {
    candles[0].time_ms = INT64_MIN;
    candles[2].time_ms = INT64_MAX;
  } else if (strcmp(mode, "large-price") == 0) {
    candles[0] = (UmiChartCandle){1000, DBL_MAX / 2, DBL_MAX, 0, DBL_MAX, 1};
    count = 1U;
    last = 0U;
  } else if (strcmp(mode, "percent-overflow") == 0) {
    candles[0] = (UmiChartCandle){1000, DBL_MIN, DBL_MAX, 0, DBL_MAX, 1};
    count = 1U;
    last = 0U;
  } else if (strcmp(mode, "values") != 0) {
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "bounds") == 0)
      last = SIZE_MAX;
    else if (strcmp(mode, "empty") == 0) {
      count = 0U;
      expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "capacity") == 0) {
      count = UMI_CHART_MAX_POINTS + 1U;
      expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "nan") == 0)
      candles[2].volume = NAN;
    else if (strcmp(mode, "infinite") == 0)
      candles[2].high = INFINITY;
    else if (strcmp(mode, "negative-volume") == 0)
      candles[2].volume = -1;
    else if (strcmp(mode, "ohlc") == 0)
      candles[2].low = 102;
    else if (strcmp(mode, "duplicate") == 0)
      candles[2].time_ms = 2000;
    else if (strcmp(mode, "unsorted") == 0)
      candles[2].time_ms = 1500;
    else if (strcmp(mode, "outside-corrupt") == 0) {
      last = 0U;
      candles[2].close = NAN;
    } else if (strcmp(mode, "volume-overflow") == 0) {
      candles[0].volume = DBL_MAX;
      candles[1].volume = DBL_MAX;
      expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(mode, "range-overflow") == 0) {
      candles[0].low = -DBL_MAX;
      candles[2].high = DBL_MAX;
      expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(mode, "change-overflow") == 0) {
      candles[0].open = candles[0].low = -DBL_MAX;
      candles[2].close = candles[2].high = DBL_MAX;
      expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(mode, "null") == 0) {
      CHECK(UmiChartCandleRangeAnalyze(NULL, 3, 0, 2, &range) == expected);
      CHECK(UmiChartCandleRangeAnalyze(candles, 3, 0, 2, NULL) == expected);
      CHECK(memcmp(previous, &range, sizeof(range)) == 0);
      return 0;
    } else
      return 2;
    CHECK(UmiChartCandleRangeAnalyze(candles, count, first, last, &range) == expected);
    CHECK(memcmp(previous, &range, sizeof(range)) == 0);
    return 0;
  }
  CHECK(UmiChartCandleRangeAnalyze(candles, count, first, last, &range) == UMI_STATUS_OK);
  if (strcmp(mode, "single") == 0) {
    CHECK(range.count == 1 && range.open == 105 && range.close == 101 && range.volume == 30);
    CHECK(range.elapsed_ms == 0 && range.largest_step_ms == 0 && range.falling == 1);
  } else if (strcmp(mode, "large-price") == 0)
    CHECK(range.has_percent_change && range.percent_change == 100);
  else if (strcmp(mode, "percent-overflow") == 0)
    CHECK(!range.has_percent_change && isfinite(range.change));
  else {
    CHECK(range.count == 3 && range.volume == 100 && range.rising == 1 && range.falling == 1 &&
          range.unchanged == 1);
    CHECK(range.first_index == 0 && range.last_index == 2 && range.close == 101 &&
          range.high == 112);
    if (strcmp(mode, "zero-open") == 0 || strcmp(mode, "negative-open") == 0)
      CHECK(!range.has_percent_change);
    else {
      CHECK(range.has_percent_change && fabs(range.percent_change - 1.0) < 1e-12);
      CHECK(range.change == 1 && range.price_range == 17 && range.low == 95);
    }
    if (strcmp(mode, "extreme-time") == 0)
      CHECK(range.elapsed_ms == UINT64_MAX);
    else
      CHECK(range.elapsed_ms == 7000 && range.largest_step_ms == 6000);
  }
  return 0;
}
