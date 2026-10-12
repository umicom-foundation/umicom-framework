/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/range_capture.c
 * PURPOSE: Keep chart measurement evidence stable while providers continue streaming.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/range_capture.h"
#include "umicom/chart/candle_inspection.h"
#include <stdlib.h>
#include <string.h>

struct UmiChartRangeCapture {
  UmiChartCandle *candles;
  size_t count, anchor;
  int complete;
  UmiChartCandleRange result;
};
UmiChartRangeCapture *UmiChartRangeCaptureCreate(void) {
  /* Allocate the larger candle array only when the user actually captures a
   * window. Empty chart panels therefore keep a small ownership object. */
  return calloc(1U, sizeof(UmiChartRangeCapture));
}
void UmiChartRangeCaptureClear(UmiChartRangeCapture *capture) {
  if (capture == NULL)
    return;
  free(capture->candles);
  memset(capture, 0, sizeof(*capture));
}
void UmiChartRangeCaptureDestroy(UmiChartRangeCapture *capture) {
  if (capture == NULL)
    return;
  UmiChartRangeCaptureClear(capture);
  free(capture);
}
UmiStatus UmiChartRangeCaptureBegin(UmiChartRangeCapture *capture, const UmiChartCandle *candles,
                                    size_t count, size_t first, size_t visible_count,
                                    int64_t requested_time_ms) {
  if (capture == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiChartCandleInspection anchor;
  UmiStatus status =
      UmiChartCandleInspect(candles, count, first, visible_count, requested_time_ms, &anchor);
  if (status != UMI_STATUS_OK)
    return status;
  /* Inspection bounded the count and multiplication. Stage the replacement
   * before releasing old evidence, including when the caller aliases it. */
  UmiChartCandle *copy = malloc(visible_count * sizeof(*copy));
  if (copy == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(copy, candles + first, visible_count * sizeof(*copy));
  UmiChartRangeCaptureClear(capture);
  capture->candles = copy;
  capture->count = visible_count;
  capture->anchor = anchor.index - first;
  return UMI_STATUS_OK;
}
UmiStatus UmiChartRangeCaptureComplete(UmiChartRangeCapture *capture, int64_t requested_time_ms,
                                       UmiChartCandleRange *out) {
  if (capture == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (capture->count == 0U)
    return UMI_STATUS_INVALID_STATE;
  UmiChartCandleInspection endpoint;
  UmiStatus status = UmiChartCandleInspect(capture->candles, capture->count, 0U, capture->count,
                                           requested_time_ms, &endpoint);
  UmiChartCandleRange result;
  if (status == UMI_STATUS_OK)
    status = UmiChartCandleRangeAnalyze(capture->candles, capture->count, capture->anchor,
                                        endpoint.index, &result);
  if (status != UMI_STATUS_OK)
    return status;
  capture->result = result;
  capture->complete = 1;
  *out = result;
  return UMI_STATUS_OK;
}
UmiStatus UmiChartRangeCaptureRead(const UmiChartRangeCapture *capture, UmiChartCandleRange *out) {
  if (capture == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!capture->complete)
    return UMI_STATUS_NOT_FOUND;
  *out = capture->result;
  return UMI_STATUS_OK;
}
UmiStatus UmiChartRangeCaptureCandleAt(const UmiChartRangeCapture *capture, size_t index,
                                       UmiChartCandle *out) {
  if (capture == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!capture->complete || index >= capture->result.count)
    return UMI_STATUS_NOT_FOUND;
  *out = capture->candles[capture->result.first_index + index];
  return UMI_STATUS_OK;
}
