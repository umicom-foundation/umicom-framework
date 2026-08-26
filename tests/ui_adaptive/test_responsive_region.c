/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_responsive_region.c
 *
 * PURPOSE:
 *   Validate define size-class aware semantic shell regions with priority-based presentation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/responsive_region.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveResponsiveRegion region;
    CHECK(umi_adaptive_responsive_region_init(&region, "inspector", UMI_ADAPTIVE_REGION_INSPECTOR, UMI_DESIGN_SIZE_EXPANDED, UMI_DESIGN_SIZE_WIDE, 50U) == UMI_STATUS_OK);
    CHECK(umi_adaptive_responsive_region_matches(&region, UMI_DESIGN_SIZE_WIDE));
    CHECK(!umi_adaptive_responsive_region_matches(&region, UMI_DESIGN_SIZE_COMPACT));
    return 0;
}
