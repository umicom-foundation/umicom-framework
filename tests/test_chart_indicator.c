/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_chart_indicator.c
 *
 * PURPOSE:
 *   Verify one part of the Web Server and multi-frontend platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small test exercises the public contract directly so a beginner can see the expected behaviour without starting a complete Umicom product.
 */

#include "umicom/umicom.h"
#include "value_archive/construction_cases.h"
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted series value. */
static int UmiChartSeriesConstructionCases(void)
{
    UmiChartSeries *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_chart_series_init(value, "close", UMI_CHART_LINE) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_chart_series_init(value, oversized, UMI_CHART_LINE) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_chart_series_init(value, value->id, UMI_CHART_LINE) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "close") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiChartSeriesConstructionCases() != 0) return 1;

    UmiChartSeries input;
    UmiChartSeries output;
    int index;

    assert(umi_chart_series_init(&input, "close", UMI_CHART_LINE) ==
        UMI_STATUS_OK);
    /* Add a small predictable sequence so both study formulas can be checked
     * without depending on a graphical renderer. */
    for (index = 0; index < 5; ++index) {
        assert(umi_chart_series_add(
            &input,
            (UmiChartPoint){index, (double)(index + 1)}) == UMI_STATUS_OK);
    }
    assert(umi_chart_indicator_sma(&input, 3U, &output) == UMI_STATUS_OK);
    assert(output.point_count == 3U);
    assert(output.points[0].value == 2.0);
    assert(output.points[2].value == 4.0);
    assert(umi_chart_indicator_ema(&input, 3U, &output) == UMI_STATUS_OK);
    assert(output.point_count == 5U);
    assert(output.points[0].value == 1.0);
    assert(output.points[4].value == 4.0625);
    assert(umi_chart_indicator_sma(&input, 0U, &output) ==
        UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_chart_indicator_sma(&input, 3U, &input) ==
        UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
