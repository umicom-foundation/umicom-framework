/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_chart_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt chart presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/chart_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveChartDecision d = umi_adaptive_chart_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT);
    CHECK(!d.legend_visible && d.minimum_height == 220);
    return 0;
}
