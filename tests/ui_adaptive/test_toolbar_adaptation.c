/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_toolbar_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt toolbar presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/toolbar_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveToolbarDecision d = umi_adaptive_toolbar_adaptation_resolve(8U, UMI_DESIGN_SIZE_COMPACT);
    CHECK(d.visible_commands == 3U && d.overflow);
    return 0;
}
