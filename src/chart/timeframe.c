/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/timeframe.c
 * PURPOSE: Derive retained-data chart projections without changing provider history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/timeframe.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int UmiChartTimeframeValid(uint32_t interval_ms)
{
    return interval_ms == 0U || interval_ms == 60000U || interval_ms == 300000U ||
        interval_ms == 900000U || interval_ms == 3600000U || interval_ms == 14400000U || interval_ms == 86400000U;
}
const char *UmiChartTimeframeName(uint32_t interval_ms)
{
    switch (interval_ms) {
    case 0: return "Source"; case 60000: return "1 minute"; case 300000: return "5 minutes";
    case 900000: return "15 minutes"; case 3600000: return "1 hour"; case 14400000: return "4 hours";
    case 86400000: return "1 day (UTC)"; default: return "Unsupported interval";
    }
}
/* Floor division retains correct UTC buckets before the Unix epoch. Reject
 * the unrepresentable bucket immediately below INT64_MIN instead of wrapping. */
static UmiStatus Bucket(int64_t time, uint32_t interval, int64_t *out)
{
    int64_t size = interval, remainder = time % size;
    if (remainder < 0) remainder += size;
    if (time < INT64_MIN + remainder) return UMI_STATUS_CAPACITY_EXCEEDED;
    *out = time - remainder; return UMI_STATUS_OK;
}
UmiStatus UmiChartTimeframeAggregate(const UmiChartObservedBar *source, size_t count,
    uint32_t interval_ms, UmiChartCandle *out, size_t capacity, UmiChartTimeframeSummary *summary)
{
    if (summary == NULL || (count != 0U && source == NULL) || (capacity != 0U && out == NULL) ||
        !UmiChartTimeframeValid(interval_ms)) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_CHART_MAX_POINTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartTimeframeSummary result = {count, 0U, interval_ms};
    if (count == 0U) { *summary = result; return UMI_STATUS_OK; }
    UmiChartCandle *candles = calloc(count, sizeof *candles);
    if (candles == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0; i < count; ++i) {
        const UmiChartObservedBar *bar = &source[i];
        status = umi_chart_candle_validate(&bar->candle);
        if (status != UMI_STATUS_OK) break;
        if (bar->end_ms < bar->candle.time_ms || (i != 0U &&
            (bar->candle.time_ms <= source[i - 1U].candle.time_ms ||
             (interval_ms != 0U && source[i - 1U].end_ms > bar->candle.time_ms)))) {
            status = UMI_STATUS_INVALID_ARGUMENT; break;
        }
        int64_t bucket = bar->candle.time_ms;
        if (interval_ms != 0U) {
            status = Bucket(bar->candle.time_ms, interval_ms, &bucket);
            if (status != UMI_STATUS_OK) break;
            /* Compare using unsigned distance so INT64 endpoints cannot overflow.
             * An observation ending exactly at the boundary remains in its start bucket. */
            if ((uint64_t)bar->end_ms - (uint64_t)bucket > interval_ms) {
                status = UMI_STATUS_UNAVAILABLE; break;
            }
        }
        if (result.candle_count == 0U || candles[result.candle_count - 1U].time_ms != bucket) {
            candles[result.candle_count] = bar->candle;
            candles[result.candle_count++].time_ms = bucket;
        } else {
            UmiChartCandle *current = &candles[result.candle_count - 1U];
            if (bar->candle.volume > DBL_MAX - current->volume) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
            current->high = fmax(current->high, bar->candle.high); current->low = fmin(current->low, bar->candle.low);
            current->close = bar->candle.close; current->volume += bar->candle.volume;
            if (!isfinite(current->volume)) { status = UMI_STATUS_CAPACITY_EXCEEDED; break; }
        }
    }
    if (status == UMI_STATUS_OK && result.candle_count > capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) {
        memcpy(out, candles, result.candle_count * sizeof *out); *summary = result;
    }
    free(candles); return status;
}
UmiStatus UmiChartTimeframeFormatUtc(int64_t time_ms, int dateOnly, char *out, size_t capacity)
{
    if (out == NULL || (dateOnly != 0 && dateOnly != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    int64_t day = time_ms / 86400000, within = time_ms % 86400000;
    if (within < 0) { --day; within += 86400000; }
    /* March-based Gregorian eras avoid libc timezone and limited time_t ranges.
     * Division uses nonnegative day-of-era values, including pre-epoch dates. */
    int64_t z = day + 719468, era = (z >= 0 ? z : z - 146096) / 146097;
    unsigned doe = (unsigned)(z - era * 146097);
    unsigned yoe = (doe - doe / 1460U + doe / 36524U - doe / 146096U) / 365U;
    int64_t year = (int64_t)yoe + era * 400;
    unsigned doy = doe - (365U * yoe + yoe / 4U - yoe / 100U);
    unsigned mp = (5U * doy + 2U) / 153U, date = doy - (153U * mp + 2U) / 5U + 1U;
    unsigned month = mp < 10U ? mp + 3U : mp - 9U; year += month <= 2U;
    char text[64];
    int n = dateOnly ? snprintf(text, sizeof text, "%04lld-%02u-%02u", (long long)year, month, date) :
        snprintf(text, sizeof text, "%04lld-%02u-%02u %02u:%02u", (long long)year, month, date,
            (unsigned)(within / 3600000), (unsigned)((within / 60000) % 60));
    if (n < 0 || (size_t)n >= sizeof text) return UMI_STATUS_INVALID_STATE;
    if ((size_t)n >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, text, (size_t)n + 1U); return UMI_STATUS_OK;
}
