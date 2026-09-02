/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_chart_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt chart presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/chart_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveChartDecision d = umi_adaptive_chart_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT);
    CHECK(!d.legend_visible && d.minimum_height == 220);
    return 0;
}
