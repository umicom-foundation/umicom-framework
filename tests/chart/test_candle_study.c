/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_candle_study.c
 * PURPOSE: Check independent expected values, gaps and transactional study outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/candle_study.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *test = argv[1];
    UmiChartCandle bars[5] = {{0, 1, 2, 0, 1, 1},
                              {10, 2, 3, 1, 2, 2},
                              {20, 3, 4, 2, 3, 3},
                              {30, 4, 5, 3, 4, 4},
                              {40, 5, 6, 4, 5, 5}};
    UmiChartCandleStudy *out = calloc(1U, sizeof(*out)), *before = calloc(1U, sizeof(*before));
    CHECK(out && before);
    out->count = 117U;
    *before = *out;
    UmiChartCandleStudyKind kind = UMI_CHART_CANDLE_STUDY_VOLUME_WEIGHTED;
    size_t count = 5U, period = 3U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(test, "weighted") == 0 || strcmp(test, "zero-window") == 0 ||
        strcmp(test, "large-volume") == 0 || strcmp(test, "negative-price") == 0 ||
        strcmp(test, "zero-price") == 0)
    {
    }
    else if (strcmp(test, "bollinger") == 0 || strcmp(test, "constant") == 0 || strcmp(test, "overflow") == 0)
        kind = UMI_CHART_CANDLE_STUDY_BOLLINGER;
    else if (strcmp(test, "donchian") == 0 || strcmp(test, "large-price") == 0)
        kind = UMI_CHART_CANDLE_STUDY_DONCHIAN;
    else if (strcmp(test, "duplicate-time") == 0)
    {
        bars[1].time_ms = 0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "unsorted") == 0)
    {
        bars[1].time_ms = -1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "nan") == 0)
    {
        bars[1].close = NAN;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "negative-volume") == 0)
    {
        bars[1].volume = -1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "invalid-ohlc") == 0)
    {
        bars[1].high = 0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "period-small") == 0)
    {
        period = 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "period-large") == 0)
    {
        period = 201U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "kind") == 0)
    {
        kind = (UmiChartCandleStudyKind)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "short") == 0)
        count = 2U;
    else if (strcmp(test, "empty") == 0)
        count = 0U;
    else if (strcmp(test, "capacity") == 0)
    {
        count = UMI_CHART_MAX_POINTS + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else
        CHECK(false);
    if (strcmp(test, "zero-window") == 0)
        for (size_t i = 0U; i < 3U; ++i)
            bars[i].volume = 0;
    if (strcmp(test, "large-volume") == 0)
        for (size_t i = 0U; i < 5U; ++i)
            bars[i].volume = DBL_MAX;
    if (strcmp(test, "negative-price") == 0)
        for (size_t i = 0U; i < 5U; ++i)
        {
            bars[i].open = -bars[i].open;
            bars[i].close = -bars[i].close;
            double low = bars[i].low;
            bars[i].low = -bars[i].high;
            bars[i].high = -low;
        }
    if (strcmp(test, "constant") == 0 || strcmp(test, "large-price") == 0 || strcmp(test, "zero-price") == 0)
        for (size_t i = 0U; i < 5U; ++i)
            bars[i].open = bars[i].high = bars[i].low = bars[i].close =
                strcmp(test, "large-price") == 0  ? DBL_MAX
                : strcmp(test, "zero-price") == 0 ? 0.0
                                                  : 42.0;
    if (strcmp(test, "overflow") == 0)
    {
        for (size_t i = 0U; i < 5U; ++i)
            bars[i].open = bars[i].high = bars[i].low = bars[i].close = i % 2U ? DBL_MAX : -DBL_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    UmiChartCandle original[5];
    memcpy(original, bars, sizeof(bars));
    CHECK(UmiChartCandleStudyCompute(bars, count, kind, period, out) == expected);
    CHECK(memcmp(original, bars, sizeof(bars)) == 0);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(out, before, sizeof(*out)) == 0);
    else if (count < period)
        CHECK(out->count == 0U);
    else
    {
        CHECK(out->count == 3U && out->samples[0].time_ms == 20 && out->samples[2].time_ms == 40);
        if (strcmp(test, "weighted") == 0)
            CHECK(fabs(out->samples[0].centre - 14.0 / 6.0) < 1e-12 && !out->bands);
        if (strcmp(test, "negative-price") == 0)
            CHECK(fabs(out->samples[0].centre + 14.0 / 6.0) < 1e-12);
        if (strcmp(test, "large-volume") == 0)
            CHECK(fabs(out->samples[0].centre - 2.0) < 1e-12);
        if (strcmp(test, "zero-window") == 0)
            CHECK(!out->samples[0].valid && out->samples[1].valid && out->samples[1].centre == 4.0);
        if (strcmp(test, "zero-price") == 0)
            CHECK(out->samples[0].valid && out->samples[0].centre == 0.0);
        if (strcmp(test, "constant") == 0)
            CHECK(out->samples[0].centre == 42.0 && out->samples[0].lower == 42.0 &&
                  out->samples[0].upper == 42.0);
        if (strcmp(test, "large-price") == 0)
            CHECK(out->samples[0].centre == DBL_MAX && out->samples[0].lower == DBL_MAX &&
                  out->samples[0].upper == DBL_MAX);
        if (strcmp(test, "bollinger") == 0)
            CHECK(out->bands && fabs(out->samples[0].centre - 2.0) < 1e-12 &&
                  fabs(out->samples[0].lower - 0.367006838144548) < 1e-12 &&
                  fabs(out->samples[0].upper - 3.632993161855452) < 1e-12);
        if (strcmp(test, "donchian") == 0)
            CHECK(out->bands && out->samples[0].lower == 0.0 && out->samples[0].upper == 4.0 &&
                  out->samples[0].centre == 2.0);
    }
    free(out);
    free(before);
    return 0;
}
