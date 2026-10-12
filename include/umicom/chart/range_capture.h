/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/range_capture.h
 * PURPOSE: Own candle evidence across two-point chart measurement and later review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_RANGE_CAPTURE_H
#define UMICOM_CHART_RANGE_CAPTURE_H
#include "umicom/chart/candle_range.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Single-owner, heap-backed evidence. Hosts serialize access and keep instrument
 * and timeframe identity alongside this object. No provider or widget is borrowed. */
typedef struct UmiChartRangeCapture UmiChartRangeCapture;

/** Allocate an empty capture. Returns NULL if storage cannot be allocated. */
UmiChartRangeCapture *UmiChartRangeCaptureCreate(void);
/** Release all evidence; NULL is accepted. */
void UmiChartRangeCaptureDestroy(UmiChartRangeCapture *capture);
/** Forget pending and completed measurements without touching caller data. */
void UmiChartRangeCaptureClear(UmiChartRangeCapture *capture);
/** Copy the visible window and choose its nearest starting candle.
 * The entire input is validated. Ties choose the earlier candle. A request
 * outside the visible opening times snaps to its nearest end. Failure preserves
 * the previous capture; success replaces it and clears any completed result. */
UmiStatus UmiChartRangeCaptureBegin(UmiChartRangeCapture *capture, const UmiChartCandle *candles,
                                    size_t count, size_t first, size_t visible_count,
                                    int64_t requested_time_ms);
/** Choose the nearest ending candle in the owned window and measure inclusively.
 * New provider data cannot change the evidence. Result indices address the copied
 * window, starting at zero. Failure leaves both capture and out unchanged.
 * Complete may be called again to choose a different endpoint in the same capture. */
UmiStatus UmiChartRangeCaptureComplete(UmiChartRangeCapture *capture, int64_t requested_time_ms,
                                       UmiChartCandleRange *out);
/** Copy a completed result; NOT_FOUND leaves out unchanged when none exists. */
UmiStatus UmiChartRangeCaptureRead(const UmiChartRangeCapture *capture, UmiChartCandleRange *out);
/** Copy one selected candle in chronological order after completion.
 * index is relative to the measured interval, not the original source array.
 * This supports export without borrowing a provider cache. */
UmiStatus UmiChartRangeCaptureCandleAt(const UmiChartRangeCapture *capture, size_t index,
                                       UmiChartCandle *out);

#ifdef __cplusplus
}
#endif
#endif
