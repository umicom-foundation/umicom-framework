/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_panel_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt panel presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/panel_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptivePanelDecision d = umi_adaptive_panel_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT, UMI_UI_PLACEMENT_RIGHT);
    CHECK(d.presentation == UMI_ADAPTIVE_PRESENTATION_OVERLAY && d.placement == UMI_UI_PLACEMENT_FLOATING);
    return 0;
}
