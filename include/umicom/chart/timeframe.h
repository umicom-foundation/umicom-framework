/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/timeframe.h
 * PURPOSE: Aggregate retained OHLCV observations into explicit fixed UTC chart intervals.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_TIMEFRAME_H
#define UMICOM_CHART_TIMEFRAME_H
#include "umicom/chart/candle.h"
#ifdef __cplusplus
extern "C" {
#endif
/** end_ms is the provider's observed bar end. An end exactly on the next
 * interval boundary is accepted (exclusive-end providers). A one-point event
 * may have end_ms == candle.time_ms. No missing prices or volume are invented. */
typedef struct UmiChartObservedBar { UmiChartCandle candle; int64_t end_ms; } UmiChartObservedBar;
typedef struct UmiChartTimeframeSummary {
    size_t source_count, candle_count;
    uint32_t interval_ms;
} UmiChartTimeframeSummary;
/** Supported fixed durations: source=0, 1m, 5m, 15m, 1h, 4h and 1d.
 * Daily buckets start at midnight UTC, without exchange-session/DST rules. */
int UmiChartTimeframeValid(uint32_t interval_ms);
const char *UmiChartTimeframeName(uint32_t interval_ms);
/** Preserve original candles at interval 0. Otherwise use first open, greatest
 * high, least low, last close and summed volume in each UTC-aligned bucket.
 * Require ordered unique starts. Aggregation also requires nonoverlapping
 * observed bars; source mode retains provider overlaps unchanged. A source bar
 * crossing a requested bucket returns UNAVAILABLE: it cannot be split safely.
 * Gaps stay gaps; edge buckets may be incomplete and are not certified closed.
 * Count is at most UMI_CHART_MAX_POINTS. Empty input is valid. All outputs stay
 * unchanged on failure; no overlap with inputs. Output capacity is in candles. */
UmiStatus UmiChartTimeframeAggregate(const UmiChartObservedBar *source, size_t count,
    uint32_t interval_ms, UmiChartCandle *out, size_t capacity, UmiChartTimeframeSummary *summary);
/** Format a signed Unix millisecond timestamp in UTC. dateOnly prints the
 * Gregorian date; otherwise append HH:MM. No locale or process timezone use.
 * Failure leaves output unchanged; capacity includes the terminating NUL. */
UmiStatus UmiChartTimeframeFormatUtc(int64_t time_ms, int dateOnly, char *out, size_t capacity);
#ifdef __cplusplus
}
#endif
#endif
