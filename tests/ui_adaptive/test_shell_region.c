/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_shell_region.c
 *
 * PURPOSE:
 *   Validate describe one adaptive application-shell region and its collapse/overlay capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_region.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveResponsiveRegion responsive;
    UmiAdaptiveShellRegion region;
    CHECK(umi_adaptive_responsive_region_init(&responsive, "nav", UMI_ADAPTIVE_REGION_NAVIGATION, UMI_DESIGN_SIZE_COMPACT, UMI_DESIGN_SIZE_WIDE, 10U) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_region_init(&region, &responsive, 280, 1, 1) == UMI_STATUS_OK);
    CHECK(region.preferred_extent == 280 && region.overlay_capable);
    return 0;
}
