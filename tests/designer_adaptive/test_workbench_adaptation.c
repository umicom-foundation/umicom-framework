/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_workbench_adaptation.c
 *
 * PURPOSE:
 *   Validate plan adaptive workbench side/bottom regions for desktop, tablet and handset previews.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/workbench_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerWorkbenchAdaptation w=umi_designer_workbench_adaptation_resolve(UMI_DESIGN_SIZE_WIDE);
    CHECK(w.left_inline&&w.right_inline&&w.bottom_inline);
    w=umi_designer_workbench_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT); CHECK(w.auxiliary_overlay&&!w.left_inline);
    return 0;
}
