/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_candle_inspection.c
 * PURPOSE: Check observed-candle selection, extreme timestamps and unchanged results on failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/candle_inspection.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)

int main(int argc, char **argv) {
  CHECK(argc == 2);
  UmiChartCandle candles[] = {
      {0, 10, 12, 8, 11, 0}, {100, 11, 15, 10, 14, 23}, {1000, -4, 0, -8, -1, 25}};
  UmiChartCandleInspection out;
  memset(&out, 0x5a, sizeof(out));
  unsigned char unchanged[sizeof(out)];
  memcpy(unchanged, &out, sizeof(out));
  const char *mode = argv[1];
  if (strcmp(mode, "exact") == 0) {
    CHECK(UmiChartCandleInspect(candles, 3U, 0U, 3U, 100, &out) == UMI_STATUS_OK);
    CHECK(out.index == 1U && out.distance_ms == 0U && out.candle.volume == 23);
  } else if (strcmp(mode, "tie") == 0) {
    CHECK(UmiChartCandleInspect(candles, 3U, 0U, 3U, 550, &out) == UMI_STATUS_OK);
    CHECK(out.index == 1U && out.distance_ms == 450U && out.requested_time_ms == 550);
  } else if (strcmp(mode, "gap") == 0) {
    CHECK(UmiChartCandleInspect(candles, 3U, 0U, 3U, 900, &out) == UMI_STATUS_OK);
    CHECK(out.index == 2U && out.distance_ms == 100U && out.candle.close == -1);
  } else if (strcmp(mode, "window") == 0) {
    CHECK(UmiChartCandleInspect(candles, 3U, 1U, 1U, -200, &out) == UMI_STATUS_OK);
    CHECK(out.index == 1U && out.distance_ms == 300U);
    CHECK(UmiChartCandleInspect(candles, 3U, 0U, 1U, 900, &out) == UMI_STATUS_OK);
    CHECK(out.index == 0U && out.candle.volume == 0);
  } else if (strcmp(mode, "extreme") == 0) {
    candles[0].time_ms = INT64_MIN;
    candles[1].time_ms = INT64_MAX;
    CHECK(UmiChartCandleInspect(candles, 2U, 0U, 1U, INT64_MAX, &out) == UMI_STATUS_OK);
    CHECK(out.distance_ms == UINT64_MAX && out.index == 0U);
    CHECK(UmiChartCandleInspect(candles, 2U, 1U, 1U, INT64_MIN, &out) == UMI_STATUS_OK);
    CHECK(out.distance_ms == UINT64_MAX && out.index == 1U);
  } else {
    UmiStatus expected = UMI_STATUS_INVALID_ARGUMENT;
    size_t count = 3U, first = 0U, visible = 3U;
    if (strcmp(mode, "empty") == 0) {
      count = 0U;
      visible = 0U;
      expected = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "bounds") == 0)
      first = SIZE_MAX;
    else if (strcmp(mode, "capacity") == 0) {
      count = UMI_CHART_MAX_POINTS + 1U;
      expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "nan") == 0)
      candles[2].close = NAN;
    else if (strcmp(mode, "ohlc") == 0)
      candles[2].high = -10;
    else if (strcmp(mode, "duplicate") == 0)
      candles[2].time_ms = 100;
    else if (strcmp(mode, "unsorted") == 0)
      candles[2].time_ms = 90;
    else
      return 2;
    CHECK(UmiChartCandleInspect(candles, count, first, visible, 100, &out) == expected);
    CHECK(memcmp(&out, unchanged, sizeof(out)) == 0);
  }
  return 0;
}
