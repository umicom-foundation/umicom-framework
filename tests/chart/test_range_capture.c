/*-----------------------------------------------------------------------------
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 *
 * LICENCE: MIT
 *
 * Umicom Framework
 * File: tests/chart/test_range_capture.c
 * PURPOSE: Verify captured ownership across provider replacement, reversed selection and
 * cancellation. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/range_capture.h"
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
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  UmiChartCandle candles[] = {
      {0, 10, 12, 8, 11, 20}, {100, 11, 15, 10, 14, 30}, {1000, 14, 18, 13, 16, 50}};
  UmiChartRangeCapture *capture = UmiChartRangeCaptureCreate();
  CHECK(capture);
  UmiChartCandleRange range;
  memset(&range, 0x5a, sizeof(range));
  unsigned char unchanged[sizeof(range)];
  memcpy(unchanged, &range, sizeof(range));
  const char *mode = argv[1];
  if (strcmp(mode, "empty") == 0) {
    CHECK(UmiChartRangeCaptureComplete(capture, 0, &range) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiChartRangeCaptureRead(capture, &range) == UMI_STATUS_NOT_FOUND);
    CHECK(memcmp(unchanged, &range, sizeof(range)) == 0);
  } else {
    size_t first = strcmp(mode, "window") == 0 ? 1U : 0U, count = 3U - first;
    OK(UmiChartRangeCaptureBegin(capture, candles, 3, first, count,
                                 strcmp(mode, "reverse") == 0 ? 1000 : 0));
    CHECK(UmiChartRangeCaptureRead(capture, &range) == UMI_STATUS_NOT_FOUND);
    if (strcmp(mode, "owned") == 0)
      memset(candles, 0, sizeof(candles));
    int64_t endpoint = strcmp(mode, "reverse") == 0 ? 0 : strcmp(mode, "tie") == 0 ? 550 : 1000;
    OK(UmiChartRangeCaptureComplete(capture, endpoint, &range));
    CHECK(range.count == (strcmp(mode, "tie") == 0 || strcmp(mode, "window") == 0 ? 2U : 3U));
    CHECK(range.volume == (strcmp(mode, "tie") == 0 ? 50 : strcmp(mode, "window") == 0 ? 80 : 100));
    CHECK(range.close == (strcmp(mode, "tie") == 0 ? 14 : 16));
    if (strcmp(mode, "preserve") == 0) {
      candles[2].close = NAN;
      CHECK(UmiChartRangeCaptureBegin(capture, candles, 3, 0, 3, 0) == UMI_STATUS_INVALID_ARGUMENT);
      OK(UmiChartRangeCaptureRead(capture, &range));
      CHECK(range.volume == 100);
    } else if (strcmp(mode, "bounds") == 0) {
      CHECK(UmiChartRangeCaptureBegin(capture, candles, 3, SIZE_MAX, 3, 0) ==
            UMI_STATUS_INVALID_ARGUMENT);
      OK(UmiChartRangeCaptureRead(capture, &range));
      CHECK(range.volume == 100);
    } else if (strcmp(mode, "replace") == 0) {
      OK(UmiChartRangeCaptureBegin(capture, candles, 3, 1, 1, 0));
      CHECK(UmiChartRangeCaptureRead(capture, &range) == UMI_STATUS_NOT_FOUND);
      OK(UmiChartRangeCaptureComplete(capture, 10000, &range));
      CHECK(range.count == 1 && range.volume == 30);
    } else if (strcmp(mode, "clear") == 0) {
      UmiChartRangeCaptureClear(capture);
      CHECK(UmiChartRangeCaptureRead(capture, &range) == UMI_STATUS_NOT_FOUND);
      CHECK(UmiChartRangeCaptureComplete(capture, 1000, &range) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(mode, "rows") == 0) {
      UmiChartCandle row;
      OK(UmiChartRangeCaptureCandleAt(capture, 1, &row));
      CHECK(row.close == 14 && row.time_ms == 100);
      unsigned char previous[sizeof(row)];
      memcpy(previous, &row, sizeof(row));
      CHECK(UmiChartRangeCaptureCandleAt(capture, SIZE_MAX, &row) == UMI_STATUS_NOT_FOUND);
      CHECK(memcmp(previous, &row, sizeof(row)) == 0);
    } else
      CHECK(strcmp(mode, "owned") == 0 || strcmp(mode, "reverse") == 0 ||
            strcmp(mode, "tie") == 0 || strcmp(mode, "window") == 0);
  }
  UmiChartRangeCaptureDestroy(capture);
  UmiChartRangeCaptureDestroy(NULL);
  UmiChartRangeCaptureClear(NULL);
  return 0;
}
