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
#include <assert.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
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
