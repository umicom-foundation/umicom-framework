/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_grid_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt grid presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/grid_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveGridDecision d = umi_adaptive_grid_adaptation_resolve(12U, UMI_DESIGN_SIZE_MEDIUM);
    CHECK(d.visible_columns == 6U && d.horizontal_scroll);
    return 0;
}
