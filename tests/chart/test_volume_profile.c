/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_volume_profile.c
 * PURPOSE: Verify profile quantities, boundary assignments and failure atomicity with independent expected values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/volume_profile.h"
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
    UmiChartVolumeSample samples[] = {{0, 10}, {1, 30}, {2, 20}, {3, 10}, {4, 30}};
    size_t count = 5U, bins = 4U;
    double fraction = 0.70;
    UmiStatus expected = UMI_STATUS_OK;
    UmiChartVolumeProfile *result = malloc(sizeof(*result)), *before = malloc(sizeof(*before));
    CHECK(result && before);
    memset(result, 0xa5, sizeof(*result));
    memcpy(before, result, sizeof(*result));
    if (strcmp(test, "buckets") == 0)
    {
    }
    else if (strcmp(test, "tie") == 0)
    {
        samples[0].volume = 20;
        samples[3].volume = 30;
        samples[4].volume = 0;
    }
    else if (strcmp(test, "full") == 0)
        fraction = 1.0;
    else if (strcmp(test, "half") == 0)
        fraction = 0.5;
    else if (strcmp(test, "flat") == 0)
        for (size_t i = 0; i < count; ++i)
            samples[i].price = -42;
    else if (strcmp(test, "zero") == 0)
        for (size_t i = 0; i < count; ++i)
            samples[i].volume = 0;
    else if (strcmp(test, "empty") == 0)
        count = 0U;
    else if (strcmp(test, "unordered") == 0)
    {
        UmiChartVolumeSample t = samples[0];
        samples[0] = samples[4];
        samples[4] = t;
    }
    else if (strcmp(test, "negative") == 0)
        for (size_t i = 0; i < count; ++i)
            samples[i].price -= 10;
    else if (strcmp(test, "overflow") == 0)
    {
        samples[0].volume = DBL_MAX;
        samples[1].volume = DBL_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(test, "range-overflow") == 0)
    {
        samples[0].price = -DBL_MAX;
        samples[4].price = DBL_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(test, "collapsed-edge") == 0)
    {
        count = 2;
        samples[0].price = 1e15;
        samples[1].price = nextafter(1e15, INFINITY);
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else if (strcmp(test, "nan-price") == 0)
    {
        samples[0].price = NAN;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "nan-volume") == 0)
    {
        samples[0].volume = NAN;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "negative-volume") == 0)
    {
        samples[0].volume = -1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "small") == 0)
    {
        bins = 1;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "large") == 0)
    {
        bins = 201;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "fraction-zero") == 0)
    {
        fraction = 0;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "fraction-large") == 0)
    {
        fraction = 1.01;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "fraction-nan") == 0)
    {
        fraction = NAN;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    else if (strcmp(test, "capacity") == 0)
    {
        count = UMI_CHART_MAX_POINTS + 1U;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else
        CHECK(false);
    UmiChartVolumeSample unchanged[5];
    memcpy(unchanged, samples, sizeof(samples));
    CHECK(UmiChartVolumeProfileCompute(samples, count, bins, fraction, result) == expected);
    CHECK(memcmp(unchanged, samples, sizeof(samples)) == 0);
    if (expected != UMI_STATUS_OK)
        CHECK(memcmp(before, result, sizeof(*result)) == 0);
    else if (strcmp(test, "empty") == 0 || strcmp(test, "zero") == 0)
        CHECK(!result->has_volume && result->total_volume == 0);
    else
    {
        CHECK(result->has_volume && fabs(result->total_volume - 100.0) < 1e-9);
        double total = 0;
        for (size_t i = 0; i < result->bin_count; ++i)
            total += result->bins[i].volume;
        CHECK(fabs(total - result->total_volume) < 1e-9 && result->area_volume + 1e-9 >= 100 * fraction);
        if (strcmp(test, "flat") == 0)
            CHECK(result->bin_count == 1U && result->control_price == -42 && result->area_low == -42 &&
                  result->area_high == -42);
        else if (strcmp(test, "tie") == 0)
            CHECK(result->control_bin == 1U && result->area_first == 0U && result->area_last == 2U &&
                  fabs(result->area_volume - 70) < 1e-9);
        else
        {
            CHECK(result->bin_count == 4U && result->control_bin == 3U);
            CHECK(fabs(result->bins[0].volume - 10) < 1e-9 && fabs(result->bins[1].volume - 30) < 1e-9 &&
                  fabs(result->bins[2].volume - 20) < 1e-9 && fabs(result->bins[3].volume - 40) < 1e-9);
            CHECK(result->area_first == (fraction == 1.0   ? 0U
                                         : fraction == 0.5 ? 2U
                                                           : 1U) &&
                  result->area_last == 3U);
            CHECK(fabs(result->control_price - (strcmp(test, "negative") == 0 ? -6.5 : 3.5)) < 1e-9);
        }
    }
    free(result);
    free(before);
    return 0;
}
