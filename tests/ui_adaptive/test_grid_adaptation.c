/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_grid_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt grid presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/grid_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveGridDecision d = umi_adaptive_grid_adaptation_resolve(12U, UMI_DESIGN_SIZE_MEDIUM);
    CHECK(d.visible_columns == 6U && d.horizontal_scroll);
    return 0;
}
