/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_aggregate.c
 * PURPOSE: Verify retained OHLCV aggregation, bucket bounds and atomic failure behaviour.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/timeframe.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiChartObservedBar source[12]; UmiChartCandle output[12], retained[12];
    for (int i = 0; i < 12; ++i) source[i] = (UmiChartObservedBar){
        {(int64_t)i * 60000, 100.0+i, 102.0+i, 99.0+i, 101.0+i, 10.0+i}, (int64_t)(i+1)*60000-1};
    UmiChartObservedBar untouched[12]; memcpy(untouched, source, sizeof source);
    memset(output, 0x55, sizeof output); memcpy(retained, output, sizeof output);
    UmiChartTimeframeSummary summary = {17U, 18U, 19U}, before; memcpy(&before, &summary, sizeof before);
    uint32_t interval = 300000U; size_t count = 12U, capacity = 12U; UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "source") == 0) interval = 0U;
    else if (strcmp(name, "source-overlap") == 0) { interval = 0U; source[0].end_ms = 120000; }
    else if (strcmp(name, "exclusive-sequence") == 0) {
        for (size_t i = 0; i < count; ++i) source[i].end_ms = source[i].candle.time_ms + 60000;
    }
    else if (strcmp(name, "aggregate") == 0) { /* Baseline twelve one-minute bars. */ }
    else if (strcmp(name, "empty") == 0) count = 0U;
    else if (strcmp(name, "boundary") == 0) { count = 1U; source[0].end_ms = 300000; }
    else if (strcmp(name, "crossing") == 0) { source[0].end_ms = 300001; expected = UMI_STATUS_UNAVAILABLE; }
    else if (strcmp(name, "overlap") == 0) { source[0].end_ms = 60001; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "negative") == 0) {
        count = 2U; source[0].candle.time_ms = -60000; source[0].end_ms = -1;
        source[1].candle.time_ms = 0; source[1].end_ms = 59999;
    } else if (strcmp(name, "minimum") == 0) {
        count = 1U; source[0].candle.time_ms = source[0].end_ms = INT64_MIN; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "maximum") == 0) {
        count = 1U; source[0].candle.time_ms = source[0].end_ms = INT64_MAX;
    } else if (strcmp(name, "overflow") == 0) {
        source[0].candle.volume = source[1].candle.volume = DBL_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(name, "capacity") == 0) { capacity = 2U; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (strcmp(name, "unordered") == 0) { source[1].candle.time_ms = 0; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "invalid-price") == 0) { source[11].candle.close = NAN; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "unsupported") == 0) { interval = 7U; expected = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(name, "events") == 0) {
        for (size_t i = 0; i < count; ++i) source[i].end_ms = source[i].candle.time_ms;
    } else if (strcmp(name, "gap") == 0) { source[1] = source[10]; count = 2U; }
    else return 2;
    CHECK(UmiChartTimeframeAggregate(source, count, interval, output, capacity, &summary) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(memcmp(output, retained, sizeof output) == 0 && memcmp(&summary, &before, sizeof summary) == 0);
    } else {
        CHECK(summary.source_count == count && summary.interval_ms == interval);
        if (strcmp(name, "source") == 0 || strcmp(name, "source-overlap") == 0) {
            CHECK(summary.candle_count == 12U);
            for (size_t i = 0; i < count; ++i) CHECK(memcmp(&output[i], &source[i].candle, sizeof output[i]) == 0);
        } else if (strcmp(name, "empty") == 0) CHECK(summary.candle_count == 0U && memcmp(output, retained, sizeof output) == 0);
        else if (strcmp(name, "aggregate") == 0 || strcmp(name, "events") == 0 || strcmp(name, "exclusive-sequence") == 0) {
            CHECK(summary.candle_count == 3U && output[0].time_ms == 0 && output[2].time_ms == 600000);
            CHECK(output[0].open == 100 && output[0].high == 106 && output[0].low == 99 && output[0].close == 105 && output[0].volume == 60);
            CHECK(output[1].volume == 85 && output[2].volume == 41 && output[2].close == 112);
            if (strcmp(name, "aggregate") == 0) CHECK(memcmp(source, untouched, sizeof source) == 0);
        } else if (strcmp(name, "negative") == 0) CHECK(summary.candle_count == 2 && output[0].time_ms == -300000 && output[1].time_ms == 0);
        else if (strcmp(name, "gap") == 0) CHECK(summary.candle_count == 2 && output[0].time_ms == 0 && output[1].time_ms == 600000);
        else if (strcmp(name, "boundary") == 0) CHECK(summary.candle_count == 1U && output[0].time_ms == 0);
        else CHECK(summary.candle_count == 1U && output[0].time_ms <= INT64_MAX && INT64_MAX - output[0].time_ms < 300000);
    }
    return 0;
}
